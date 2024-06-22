# 📶 LTE - using - NS3 🌐
> NS3 LTE SIMULATION

The objective of this Project is to understand and change code of LTE Schedulers algorithms in
NS-3 for necessary stats collection. Further, I also evaluated and compared the performance of
different Scheduler algorithms.

Created a topology as shown in the below figure. P-GW and Remote Host are added to this topology and
connected them with point-to-point 1 Gbps links.

![topology.png](topology.png)

## ✨Basic Configuration of eNBs and their UEs with the parameters are as follows

| 	Simulation Parameter	          | 	Value	                                                    |
| 	      :-----:	                  | 	:-----:	                                                    |
| 	    Number of UEs                 | 	10 per eNB; 1 Downlink UDP Flow per UEfrom the Remote Host	|
| 	       Number of eNBs	          | 	4	|
| 	Inter distance between eNBs	      | 	5 KM	|
| 	eNB Tx Power	                  | 	40 dBm (10 Watt)	|
| 	Application Type	              | 	UDP	|
|    Full buffer case (UDP Traffic)   |  1500 bytes per every 1ms by UDP; Each UE is configured with 1 DL UDP flow of 12Mbps|
| Non Full buffer case (UDP Traffic)  | 1500 bytes per every 10ms by UDP; EachUE is configured with 1 DL UDP flow of 1.2Mbps|
| 	UE mobility speeds	              | 0, 10 m/s; where in a given expt all UEs are configured with one of these two speeds|
| 	UE mobility model	              | 	RandomWalk2d Mobility	|
|    UEs placement in a Cell          |  Random disc placement within 500m radius of eNB|
|    # of RBs                         |  50 in DL and 50 in UL (LTE FDD) |
| UE attachment to eNB                | Automatic to one of eNBs based on received signal strength, so handovers may take place during mobility|
| 	Total simulation time	          | 	30 seconds	|
|    Number of seeds per experiment   |  5 seeds/variants |

Compared Proportional Fair (PF), Round Robin (RR), Max Throughput (MT) and Priority Set
Scheduler (PSS) available in NS-3 LENA LTE module by creating a 4-cell LTE network as
shown above with the simulation parameters given in the table.

## The following graphs have being generated for better understanding and easability of comparision of various parameters in the simulation
#### Graph 1: SINR Radio Environment Map (REM) of 4-cell topology given above.

###### I have implemented all the parameters which are crucial for the eNBs which can show vairiations in the REM plot.
I have used <RadioEnvironmentMapHelper> to assign the parameters and make it work. 
After running **Asg1** file in ns3 ques1 folder a file named **rem.out** will be generated in the same directory (checkout in [ns3 ques1](https://github.com/Harshavardhanpentakota/LTE-using-NS3/tree/main/ns3%20ques1))
which contains all the Data to generate the REM plot with X,Y positions and the SINR values, which can be done using any plotting tool. I have used gnuplot for this.

The Generated Output is here below 👇.

<img src="ns3 ques1/SINR.png"/>

The Colour Gradient Bar shows the intensity of SINR values over a particular area. For better understanding - The eNBs are placed at (0,0),(0,5000),(5000,0),(5000,5000).

#### Graph 2: X-axis: Speed (0, 10) m/s; Y-axis: (Average Aggregate System throughput) with bars for four scheduler algorithms for full buffer scenario. Getting sum of throughputs of all 4 cells (i.e., all 40 UEs flows) in different runs by varying seed values and then get the average of that for plotting.

###### Necessary implementation of all other parameters are done, here we start the actual simulation with mobility of UEs and complex implementations like Handovers, Scheduling algorithms, Seeds etc

I have used MobilityHelper to allocate the RandomDiscPositionAllocator, which Allocates spawning Position of UE randomly in a given radius and x y values of centre of the radius.
Also, ConstantPositionMobilityModel (for 0 m/s) / RandomWalk2dMobilityModel (for 10 m/s), which gives mobility to the UE in a random direction for every given unit of time.

I have also used Flowmonitor to get the Aggregate System Throughput.

To effiently run the asg2 file in a single click, i.e., without changing the seed value & scheduler parameters every time, use the following method-
-  Create a shell file in that include all the necessary command line parameters like input & output file name, schduler type, seed value etc. (checkout in [runner.sh](https://github.com/Harshavardhanpentakota/LTE-using-NS3/blob/main/ns3%20ques2%200ms/runner.sh))
-  You should run for 0ms and 10ms so in total 40 files(for 4 scheduler and 5 seed values) will be genrated, after successfull running code you will get all the aggregrate values.
-  use all the aggregrate throughtput values to plot the graph, i have used excel to plot the bar graph which is effient to plot from tables (i have used table format for better understanding).


<img src="ns3 ques2 0ms/file_2024-04-24_21.04.05(1).png"/>

###### Green line is the Average value of Aggregate System Throughputs for different seed values of same Scheduler type.

#### Graph 3: Throughput CDF plot for different schedulers at Speed (0,10) m/s for full buffer scenario; Onecurve each for 0 m/s and 10 m/s. But here you need not to do any averaging. Have list of per UE throughputs across all cells in all different runs by varying the seed value and use that for plotting CDF.

###### Cumulative Distribution Function plot for the data which we got from graph 2

i have used a basic python file which uses numpy and matplot lib to plot, it basically takes Aggregate System throughput of 0 m/s and 10 m/s as input and gives the CDF graph as output.\
checkout in [test2.py](https://github.com/Harshavardhanpentakota/LTE-using-NS3/blob/main/ns3%20ques%203/test2.py))
paste the Aggregate System throughput of 0 m/s in throughputs1 and Aggregate System throughput of 10 m/s in throughputs2.\
change the plt. plot to get the required labels in the plot.

> throughputs1 = np.array([20.4324, 20.4324, 22.3271, 20.4324, 21.3841])\
> throughputs2 = np.array([21.3841, 23.2266, 20.4324, 22.3271, 21.4559])\
> plt.plot(sorted_throughputs1, cumulative_probabilities1, marker='o', label='PSS Throughput 0m/s')\
> plt.plot(sorted_throughputs2, cumulative_probabilities2, marker='p', label='PSS Throughput 10m/s')

Here are my results 👇.

<img src="ns3 ques 3/CDF_MT.png"/>
<img src="ns3 ques 3/CDF_PSSF.png"/>
<img src="ns3 ques 3/CDF_PF.png"/>
<img src="ns3 ques 3/CDF_RR.png"/>

#### Graph 4: SINR/Instantaneous throughput values for UE 0 in the simulation for one seed (RngRun1). X-axis: Time in msec, Y-axis: SINR and Instantaneous throughputs of UE0 for Speed of 0 m/s for all fourschedulers for full buffer scenario.
###### Calculated SINR/Instantaneous throughput values for a particular UE in the simulation for Speed of 0 m/s for all fourschedulers.
###### It's a 2 line Graph, one line is for SINR values, another line is for Instantaneous Throughput values.

I have used PacketSink module to get the Total Received Bytes until that second. To calculate the Instantaneous throughput , i.e., Throughput at that Particular second, i have stored Total Received Bytes for last second and subtracted it from current Total Received Bytes. Simply\
> uint64_t presentThroughput = sink->GetTotalRx();\
> Instantaneous Throughput =  presentThroughput - prevThroughput[index]\
> prevThroughput[index] = presentThroughput;

For o m/s mobility of UE, I have used ConstantPositionMobilityModel.\
Therefore, Instantaneous Throughput with time will be printed.

For the SINR values, i have made changes in the Lte-ue-phy.cc of NS3 src documentation, It is in src > lte > model > lte-ue-phy.cc
(you can simply replace the file - checkout in [lte-ue-phy.cc](https://github.com/Harshavardhanpentakota/LTE-using-NS3/blob/main/ns3%20ques%204/lte-ue-phy.cc)
>std::cout << Simulator::Now().GetSeconds() << " " << avSinr << std::endl;

Modified so that it can print the SINR values with the time.

Command line to run the asg4 file -
> ./ns3 run asg4 > sample.txt

The SINR with time will be printed in the sample.txt whereas the Instantaneous Throughput with time will be printed in the running terminal.\ 
i have used gnuplot to combine both the files data to plot the file (checkout in [gnuplot](https://github.com/Harshavardhanpentakota/LTE-using-NS3/blob/main/ns3%20ques%204/gnuplot))\
Here's my result 👇.

<img src="ns3 ques 4/ThroughPut_MT.png"/>
<img src="ns3 ques 4/ThroughPut_PSS.png"/>
<img src="ns3 ques 4/ThroughPut_PF.png"/>
<img src="ns3 ques 4/ThroughPut_RR.png"/>

######  Priority set scheduler (PSS) allocates the resources depending upon their priority. Each UE is allocated a priority and the process with the highest priority is executed first. Therefore, Instantaneous Throughput will be greater for this algorithm. Whereas Average Aggregate System throughput will be comparatively lesser. Mostly the SINR values will be same as there are no Objects to block in the simulation and there's no mobility for UEs

#### Graph 5: SINR/Instantaneous throughput values for UE 0 in the simulation for one seed (RngRun1). X-axis: Time in msec, Y-axis: SINR and Instantaneous throughputs of UE0 for Speed of 10 m/s for all four schedulers for full buffer scenario.

###### Same graph as Graph 4 but, Here the Mobility is present, Therefore, we can see diflections in SINR values.

Just Change the Mobility model from ConstantPositionMobilityModel to RandomWalk2dMobilityModel and assign the speed values.
Run the [asg4](https://github.com/Harshavardhanpentakota/LTE-using-NS3/blob/main/ns3%20ques%204/asg4.cc) file in ns3 ques4 folder. Run it with the runner shell script to run in effiently.\
i have used gnuplot to combine both the files data to plot the file (same as in graph 4). Here's my result 👇.

<img src="ns3 ques 5/ThroughPut_MT.png"/>
<img src="ns3 ques 5/ThroughPut_PSS.png"/>
<img src="ns3 ques 5/ThroughPut_PF.png"/>
<img src="ns3 ques 5/ThroughPut_RR.png"/>


#### Graph 6: Repeat now for non full buffer scenario and report your observations. X-axis: Speed (0) m/s;Y-axis: (Average Aggregate System throughput) with bars for four scheduler algorithms.

###### This graph requires the implimentation of non full buffer in the simulation.

Just Change the interPacketInterval to 10 ms. Also change the Change the Mobility model to ConstantPositionMobilityModel.

> Time interPacketInterval = MilliSeconds (10);

Calculate Average Aggregate System throughput for four scheduler algorithms (only for 1 seed value)
Plot the graph which will be almost similar to Graph 2.\
Here's my result 👇.

<img src="ns3 ques 6/half buffer ques6.png"/>


######                                             *✨ The End - **Harsha Vardhan** ✨ *
