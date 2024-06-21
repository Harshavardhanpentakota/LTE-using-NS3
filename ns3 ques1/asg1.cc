#include <ns3/buildings-helper.h>
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"

#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include <ns3/flow-monitor-helper.h>

#include <iostream>

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("Assignment");

void UEsConnectionStatus(std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
    std::cout << context << " UE IMSI " << imsi << " connected to cellId " << cellId << " with rnti " << rnti << "Time:" << Simulator::Now() << std::endl;
}


int
main(int argc, char* argv[])
{


    //                 [Cell 1] ---------- [Cell 2]
    //                 |                   |
    //                 |                   |
    //                 |                   |
    //                 |                   |
    //             5km |                   | 5km
    //                 |                   |
    //                 |                   |
    //                 |                   |
    //                 |                   |
    //  [P-GW] --- 1 Gbps link --- [Remote Host]
    //                 |                   |
    //                 |                   |
    //                 |                   |
    //                 |                   |
    //               [Cell 3] ---------- [Cell 4]

  
    // Command line arguments
    uint32_t schedulerType = 0;
    double ueSpeed = 0.0;
    uint32_t rngRun = 9;        // Default random number generator seed
    bool fullBufferFlag = true; // Default to full buffer flag

    
    // Parse command line arguments
    CommandLine cmd;
    cmd.AddValue("schedulerType",
                 "Scheduler type (0 for PF, 1 for RR, 2 for MT, 3 for PSS)",
                 schedulerType);
    cmd.AddValue("ueSpeed", "UE mobility speed", ueSpeed);
    cmd.AddValue("rngRun", "Random number generator seed", rngRun);
    cmd.AddValue("fullBufferFlag", "Flag to indicate full buffer case", fullBufferFlag);
    cmd.Parse(argc, argv);

    // Simulation parameters
    // uint32_t numSeeds = 5;
    uint32_t rngRun1 = rngRun;
    uint32_t simulationTime = 30; // seconds

    
    // Set random number generator seed
    SeedManager::SetSeed(rngRun1);

    // Create nodes for eNBs, UE, P-GW, and Remote Host
    
    NodeContainer eNBs;
    eNBs.Create(4); // Number of eNBs = 4

    NodeContainer UE;
    UE.Create(40); // Number of UEs = 10 per eNB * 4 eNBs = 40

  //  NodeContainer pgw;
    //pgw.Create(1); // Packet Data Network Gateway
    
     // Create  Packet Data Network Gateway node
    Ptr<Node> pgw = CreateObject<Node>();

    NodeContainer remoteHost;
    remoteHost.Create(1); // Remote Host
    
    // Install the Internet stack on all nodes
    InternetStackHelper internetStack;
    internetStack.Install(eNBs);
     internetStack.Install(UE);
     internetStack.Install(pgw);
    internetStack.Install(remoteHost);
    
    // Create point-to-point connections between P-GW and Remote Host
    
    PointToPointHelper p2p;
      p2p.SetDeviceAttribute("DataRate", DataRateValue(DataRate("1Gbps")));
    p2p.SetDeviceAttribute("Mtu", UintegerValue(1500));
    p2p.SetChannelAttribute("Delay", TimeValue(Seconds(0.0))); // Adjust delay as needed

    NetDeviceContainer pgwRemoteHostDevices = p2p.Install(pgw, remoteHost.Get(0));
    
    // Connect eNBs to P-GW
    NetDeviceContainer enbDevices;
    for (uint32_t i = 0; i < eNBs.GetN(); ++i)
    {
        enbDevices.Add(p2p.Install(eNBs.Get(i), pgw));
        // enbPgwDevices = p2ph.Install (enbNodes.Get (i), pgwNode);
    }
    
       // Assign IP addresses
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("10.0.0.0", "255.0.0.0");
     ipv4h.Assign(enbDevices);
    ipv4h.Assign(pgwRemoteHostDevices);


   
    
    
      // Set mobility model for eNBs
   
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator>();
    enbPositionAlloc->Add(Vector(0, 0, 0));       // eNB 0 position
    enbPositionAlloc->Add(Vector(5000, 0, 0));    // eNB 1 position
    enbPositionAlloc->Add(Vector(0, 5000, 0));    // eNB 2 position
    enbPositionAlloc->Add(Vector(5000, 5000, 0)); // eNB 3 position
    mobility.SetPositionAllocator(enbPositionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(eNBs);
    
    
       // Set mobility model for UEs 
       MobilityHelper mobility2;
// Seed initialization
srand(time(NULL));
    Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator>();
    // Generate and print random positions for UEs

    for (uint32_t i = 0; i < UE.GetN(); ++i)
    {
        double x = rand() % 500; // Random disc placement within 500m radius of eNB
        double y = rand() % 500;
        
        
        uePositionAlloc->Add(Vector(x, y, 0));
   
    }
           mobility2.SetPositionAllocator(uePositionAlloc);
  mobility2.SetMobilityModel("ns3::RandomWalk2dMobilityModel", "Bounds", RectangleValue (Rectangle (-5000, 5000, -5000, 5000)));
  

  
    mobility2.Install(UE);
    

    
    

    // Install LTE protocol stack on eNBs, UE, P-GW
    

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    // Set scheduler type
   
    lteHelper->SetSchedulerType("ns3::PfFfMacScheduler"); // Default scheduler type
  

    // Install LTE protocol stack on eNBs
   
    NetDeviceContainer enbDevs;
    enbDevs = lteHelper->InstallEnbDevice(eNBs);

    // Install LTE protocol stack on UEs
    
    NetDeviceContainer ueDevs;
    ueDevs = lteHelper->InstallUeDevice(UE);
    
     // Attach the UEs to an eNB.
      for (uint16_t i = 0; i < 4; i++)
      {
      for (uint16_t j = 0; j < 10; j++)
        
            lteHelper->Attach (ueDevs.Get(j), enbDevs.Get(i));
        
        
      }


    // Configure eNBs parameters
    
   
    

    for (uint32_t i = 0; i < eNBs.GetN(); ++i)
    {
    
        Ptr<LteEnbPhy> enbPhy = enbDevs.Get(i)->GetObject<LteEnbNetDevice>()->GetPhy ();;
       if (enbPhy)
    {
        enbPhy->SetTxPower(40.0); // eNB Tx Power = 40 dBm
        
    }
    else
    {
        std::cerr << "Failed to cast enbDevs[" << i << "] to LteEnbNetDevice" << std::endl;
    }
    }
    

 
    // Set up applications (downlink UDP flows)
    
    

Ptr<Ipv4> ipv4 = remoteHost.Get(0)->GetObject<Ipv4>();

if (ipv4)
{
   
    // Assuming the remoteHost node has an IPv4 interface at index 1
    Ipv4Address ip = ipv4->GetAddress(1, 0).GetLocal();
   

    // Initialize UDP client helper with the retrieved IP address
    UdpClientHelper udpClientHelper(ip, 4000);
    
    // Set UDP client attributes
    udpClientHelper.SetAttribute("MaxPackets", UintegerValue(1));
    

 
    
    
  
// Full buffer case: 1500 bytes per every 1ms by UDP, 12 Mbps
if (fullBufferFlag)
{
    
    udpClientHelper.SetAttribute("Interval", TimeValue(MilliSeconds(1)));
}

// Non full buffer case: 1500 bytes per every 10ms by UDP, 1.2 Mbps
else
{
    
    udpClientHelper.SetAttribute("Interval", TimeValue(MilliSeconds(10)));
}

// Set attributes
udpClientHelper.SetAttribute("PacketSize", UintegerValue(1500));

    ApplicationContainer clientApps = udpClientHelper.Install(UE);
    clientApps.Start(Seconds(0.0));
    
    clientApps.Stop(Seconds(simulationTime));
}
else
{
    std::cerr << "IPv4 object not found for remoteHost..." << std::endl;
}

Ptr<RadioEnvironmentMapHelper> remHelper = CreateObject<RadioEnvironmentMapHelper>();
    remHelper->SetAttribute("Channel", PointerValue(lteHelper->GetDownlinkSpectrumChannel()));
    remHelper->SetAttribute("OutputFile", StringValue("rem.out"));
    remHelper->SetAttribute("XMin", DoubleValue(-20000.0));
    remHelper->SetAttribute("XMax", DoubleValue(20000.0));
    // remHelper->SetAttribute("XRes", UintegerValue(100));
    remHelper->SetAttribute("YMin", DoubleValue(-10000.0));
    remHelper->SetAttribute("YMax", DoubleValue(10000.0));
    // remHelper->SetAttribute("YRes", UintegerValue(75));
    remHelper->Install();
        

    
	lteHelper->EnableTraces ();
	 Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
                    MakeCallback(&UEsConnectionStatus));
                    
                    
	//Flow monitor
    Ptr<FlowMonitor> flowmon;
    FlowMonitorHelper flowmonHelper;
    flowmon = flowmonHelper.InstallAll();
    
    

    // Run the simulation
    Simulator::Stop(Seconds(simulationTime));
    Simulator::Run();
    
     flowmon->SerializeToXmlFile("lte_flowmon.xml", true, true);
    Simulator::Destroy();
    
    
 std::cerr << "Run the simulation is completed" << std::endl;
    
    return 0;
}
