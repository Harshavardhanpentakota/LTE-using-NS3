/* --  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -- */
/*
 * Copyright (c) 2011-2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Jaume Nin <jaume.nin@cttc.cat>
 *          Manuel Requena <manuel.requena@cttc.es>
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/lte-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "bits/stdc++.h"
#include "ns3/lte-my_mcs.h"
// #include "ns3/cqi-dl-env.h"
// #include "my-pss-ff-mac-scheduler.h"

// #include "ns3/gtk-config-store.h"

using namespace ns3;


/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeBs,
 * attaches one UE per eNodeB starts a flow for each UE to and from a remote host.
 * It also starts another flow between each UE pair.
 */

NS_LOG_COMPONENT_DEFINE ("LenaSimpleEpc");

int main (int argc, char *argv[])
{
  uint16_t numNodePairs = 20;
  Time simTime = MilliSeconds (30000);
  double distance = 60.0;
  Time interPacketInterval = MilliSeconds (10);
  bool useCa = false;
  bool disableDl = true;
  bool disableUl = false;
  bool disablePl = false;
  std::string scheduler = "ns3::RrFfMacScheduler";
  uint16_t seedvalue = 33;

  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Mode", StringValue ("Time"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Time", StringValue ("0.01s"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Speed", StringValue ("ns3::UniformRandomVariable[Min=10.0|Max=10.0]"));
  Config::SetDefault ("ns3::RandomWalk2dMobilityModel::Bounds", StringValue ("-5000|5000|-5000|5000"));
  // Command line arguments
  CommandLine cmd (__FILE__);
  cmd.AddValue ("numNodePairs", "Number of eNodeBs + UE pairs", numNodePairs);
  cmd.AddValue ("simTime", "Total duration of the simulation", simTime);
  cmd.AddValue ("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue ("interPacketInterval", "Inter packet interval", interPacketInterval);
  cmd.AddValue ("useCa", "Whether to use carrier aggregation.", useCa);
  cmd.AddValue ("disableDl", "Disable downlink data flows", disableDl);
  cmd.AddValue ("disableUl", "Disable uplink data flows", disableUl);
  cmd.AddValue ("disablePl", "Disable data flows between peer UEs", disablePl);
  cmd.AddValue ("seedvalue", "seedvalue", seedvalue);
  cmd.AddValue ("scheduler", "Scheduler", scheduler);
  cmd.Parse (argc, argv);

    ConfigStore inputConfig;
    inputConfig.ConfigureDefaults ();
    RngSeedManager::SetSeed(seedvalue);  // Changes seed from default
    RngSeedManager::SetRun(seedvalue);
    // LogComponentEnable("PssFfMacScheduler", LOG_LEVEL_ALL);
  // parse again so you can override default values from the command line
  cmd.Parse(argc, argv);

  if (useCa)
   {
     Config::SetDefault ("ns3::LteHelper::UseCa", BooleanValue (useCa));
     Config::SetDefault ("ns3::LteHelper::NumberOfComponentCarriers", UintegerValue (2));
     Config::SetDefault ("ns3::LteHelper::EnbComponentCarrierManager", StringValue ("ns3::RrComponentCarrierManager"));
   }

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  lteHelper->SetSchedulerType(scheduler);

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

   // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("1Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create (4);
  ueNodes.Create (40);

    // Install Mobility Model
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add (Vector(0, 0, 0));
    positionAlloc->Add (Vector(5000, 0, 0));
    positionAlloc->Add (Vector(0, 5000, 0));
    positionAlloc->Add (Vector(5000, 5000, 0));

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(enbNodes);


    MobilityHelper mobility2;
    mobility2.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                  "X",
                                  StringValue("0.0"),
                                  "Y",
                                  StringValue("0.0"),
                                  "Rho",
                                  StringValue("ns3::UniformRandomVariable[Min=5000|Max=5000]"));
    mobility2.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                //RandomWalk2dMobilityModel ConstantPositionMobilityModel
                                "Mode", StringValue("Time"),
                                "Time", StringValue("1s"),
                                "Speed", StringValue("ns3::UniformRandomVariable[Min=10.0|Max=10.0]"),
                                "Bounds", RectangleValue (Rectangle (-5000, 5000, -5000, 5000))
                                );
    mobility2.Install(ueNodes);

  // Install LTE Devices to the nodes
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }
    // enbLteDevs.Get(0)->SetAttribute("TxPower", DoubleValue(40.0));
  // Attach one UE per eNodeB
  for (uint16_t i = 0; i < 40; i++)
    {
        lteHelper->AttachToClosestEnb(ueLteDevs.Get(i), enbLteDevs);
      // side effect: the default EPS bearer will be activated
    }


  // Install and start applications on UEs and remote host
  uint16_t dlPort = 1100;
  uint16_t ulPort = 2000;
  uint16_t otherPort = 3000;
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      if (!disableDl)
        {
          PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
          serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get (u)));

          UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
          dlClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
          dlClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          clientApps.Add (dlClient.Install (remoteHost));
        }

      if (!disableUl)
        {
          ++ulPort;
          PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
          serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

          UdpClientHelper ulClient (remoteHostAddr, ulPort);
          ulClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
          ulClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
          clientApps.Add (ulClient.Install (ueNodes.Get(u)));
        }

      if (!disablePl && numNodePairs > 1)
        {
          ++otherPort;
          PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
          serverApps.Add (packetSinkHelper.Install (ueNodes.Get (u)));

          UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
          client.SetAttribute ("Interval", TimeValue (interPacketInterval));
          client.SetAttribute ("MaxPackets", UintegerValue (1000000));
          clientApps.Add (client.Install (ueNodes.Get ((u + 1) % numNodePairs)));
        }
        // Simulator::Schedule(Seconds(1), &func, DynamicCast<PacketSink> (serverApps.Get(u)), 1, u);

    }

  serverApps.Start (MilliSeconds (500));
  clientApps.Start (MilliSeconds (500));
  lteHelper->EnableTraces ();
  // Uncomment to enable PCAP tracing
  //p2ph.EnablePcapAll("lena-simple-epc");

  Ptr<FlowMonitor> flowmon;
  FlowMonitorHelper flowmonHelper;
  NodeContainer endPoints;
  endPoints.Add(remoteHost);
  endPoints.Add(ueNodes);
  flowmon = flowmonHelper.Install (endPoints);


  Simulator::Stop (simTime);
  Simulator::Run ();

  flowmon->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
    FlowMonitor::FlowStatsContainer stats = flowmon->GetFlowStats ();

    double averageFlowThroughput = 0.0;


    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
        std::stringstream protoStream;
        protoStream << (uint16_t) t.protocol;
        if (t.protocol == 6)
        {
            protoStream.str ("TCP");
        }
        if (t.protocol == 17)
        {
            protoStream.str ("UDP");
        }
        // std::cerr << "\n========================================================================\n";
        // std::cerr << "\nFlow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> " << t.destinationAddress << ":" << t.destinationPort << ") proto " << protoStream.str () << "\n";
        // std::cerr << "  Tx Packets: " << i->second.txPackets << "\n";
        // std::cerr << "  Tx Bytes:   " << i->second.txBytes << "\n";
        // std::cerr << "  Rx Packets: " << i->second.rxPackets << "\n";
        // std::cerr << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        // std::cerr << "  Delay Sum:  " << i->second.delaySum.GetSeconds () << "\n";
        // // std::cerr << "  TxOffered:  " << i->second.txBytes * 8.0 / (simTime) / 1000 / 1000  << " Mbps\n";
        // std::cerr << "\n------------------------------------------------------------------------\n";
        if (i->second.rxPackets > 0)
        {
            // Measure the duration of the flow from receiver's perspective
            double rxDuration = i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ();

            averageFlowThroughput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
            
            // std::cerr << "  Throughput: " << i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000  << " Mbps\n";
            // std::cerr << "  Mean delay:  " << 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets << " ms\n";
            // //std::cerr << "  Mean upt:  " << i->second.uptSum / i->second.rxPackets / 1000/1000 << " Mbps \n";
            // std::cerr << "  Mean jitter:  " << 1000 * i->second.jitterSum.GetSeconds () / i->second.rxPackets  << " ms\n";
        }
        else
        {
            // std::cerr << "  Throughput:  0 Mbps\n";
            // std::cerr << "  Mean delay:  0 ms\n";
            // std::cerr << "  Mean upt:  0  Mbps \n";
            // std::cerr << "  Mean jitter: 0 ms\n";
        }
        double packetLoss=i->second.txPackets-i->second.rxPackets ;
        double lossRate=(packetLoss/i->second.txPackets)*100;
        // std::cerr << "  Loss Rate: " <<lossRate<<"\n";

    }
    // std::cerr << "\n========================================================================\n";
    // std::cerr << "\n\n  Mean flow throughput: " << averageFlowThroughput / stats.size () << "\n";
    std::cerr << "\n\n  Aggregrate throughput: " << averageFlowThroughput<< "\n";


  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy ();
  return 0;
}