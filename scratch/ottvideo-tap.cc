/*
*
*Network topology
*
*  n1 (disturbatore)                 n4 (disturbatore)
*    \ 5 Mb/s, 2ms                  / 5 Mb/s, 2ms
*     \          20 Mb/s, 10ms     /
*      ----------------------------
*     /                            \
*    / 5 Mb/s, 2ms                  \ 5 Mb/s, 2ms
*   n0                               n5
*    |                                |
*    |                                |
*  ubuntu1                       ubuntu2
*      |                            |
*      |                            |  
*   TapLeft                      TapRight
*      ================================
*                   CSMA LAN
* 
* n0 = ubuntu1
* n5 = ubuntu2
* 
*/




#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
//#include "ns3/netanim-module.h"
//#include "ns3/flow-monitor-module.h"
//#include "ns3/flow-monitor-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/tap-bridge-module.h"
//#include "ns3/animation-interface.h"
#include "ns3/double.h"
#include "ns3/simulator.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/ipv4-address.h"
#include "ns3/address-utils.h" 
#include "ns3/inet-socket-address.h"
#include "ns3/on-off-helper.h"
#include "ns3/udp-socket-factory.h"


  using namespace ns3;
  
  Ptr<UniformRandomVariable> rndDelay, rndDataRate, rndVarTime;

void changeLink(Ptr<CsmaChannel> channel)
{
	double dataRate = rndDataRate->GetValue();
	double delay = rndDelay->GetValue();
	
	channel->SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
	channel->SetAttribute("Delay", TimeValue(MilliSeconds(delay)));

	NS_LOG_UNCOND(Now().GetMilliSeconds() << "ms " << "link delay = " << delay << "ms link rate = " << dataRate << "bps");

	Simulator::Schedule(MilliSeconds(rndVarTime->GetValue()), &changeLink, channel);
}

  int 
  main (int argc, char *argv[])
  {
	std::string mode = "ConfigureLocal";
	//Config :: SetDefault ( "ns3 :: OnOffApplication PacketSize" , UintegerValue (210));
	//Config :: SetDefault ( "ns3 :: OnOffApplication datarate" , StringValue ( "448kb / s" )); 
	
	//uint16_t port = 9;
	uint32_t simTime = 3600.0;
	double delayMax = 1000;
	double delayMin = 1;
	uint32_t dataRateMax = 100000000;
	uint32_t dataRateMin = 100000;
	double varTimeMax = 5000;
	double varTimeMin = 1;

    CommandLine cmd;
    
    Address serverAddress;
    cmd.AddValue ("mode", "Mode setting of TapBridge", mode);
    cmd.AddValue ("simTime", "SimulationTime", simTime);
    cmd.AddValue ("dataRateMax", "", dataRateMax);
    cmd.AddValue ("dataRateMin", "", dataRateMin);
    cmd.AddValue ("delayMax", "", delayMax);
    cmd.AddValue ("delayMin", "", delayMin);
    cmd.AddValue ("varTimeMax", "", varTimeMax);
    cmd.AddValue ("varTimeMin", "", varTimeMin);

    cmd.Parse (argc, argv);
  
  rndDelay = CreateObject<UniformRandomVariable> ();
  rndDelay->SetAttribute("Max", DoubleValue(delayMax));
  rndDelay->SetAttribute("Min", DoubleValue(delayMin));
  rndDataRate = CreateObject<UniformRandomVariable> ();
  rndDataRate->SetAttribute("Max", DoubleValue(dataRateMax));
  rndDataRate->SetAttribute("Min", DoubleValue(dataRateMin));
  rndVarTime = CreateObject<UniformRandomVariable> ();
  rndVarTime->SetAttribute("Max", DoubleValue(varTimeMax));
  rndVarTime->SetAttribute("Min", DoubleValue(varTimeMin));

    //
    // We are interacting with the outside, real, world.  This means we have to 
    // interact in real-time and therefore means we have to use the real-time
    // simulator and take the time to calculate checksums.
    //
    GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
    GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
  

    NodeContainer csmaNodes;
    
    Ptr<Node> n0 = CreateObject<Node>();
    csmaNodes.Add(n0);
    Ptr<Node> n1 = CreateObject<Node>();
    csmaNodes.Add(n1);
 /*   Ptr<Node> n2 = CreateObject<Node>();
    csmaNodes.Add(n2);
    Ptr<Node> n3 = CreateObject<Node>();
    csmaNodes.Add(n3); */
    Ptr<Node> n4 = CreateObject<Node>();
    csmaNodes.Add(n4);
    Ptr<Node> n5 = CreateObject<Node>();
    csmaNodes.Add(n5);

    NodeContainer csma1, csma2, csma3, csma4, csma5;
    csma1.Add(n0);
    csma1.Add(n5);
    
    csma2.Add(n1);
    csma2.Add(n5);
    
    csma3.Add(n0);
    csma3.Add(n4);
    
    csma4.Add(n1);
    csma4.Add(n4);
  
    CsmaHelper csma;
    csma.SetDeviceAttribute ("Mtu", UintegerValue (1024));
    NetDeviceContainer d0d5 = csma.Install (csma1);
    NetDeviceContainer d1d5 = csma.Install (csma2);
    NetDeviceContainer d0d4 = csma.Install (csma3);
    NetDeviceContainer d1d4 = csma.Install (csma4);
  
    Simulator::Schedule(Seconds(0.0), &changeLink, d0d5.Get(0)->GetObject<CsmaNetDevice>()->GetChannel()->GetObject<CsmaChannel>());

    InternetStackHelper internet;
    //stack.Install (csmaNodes);
    internet.Install (csmaNodes);
    
    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i5 = ipv4.Assign (d0d5);
    
   
    ipv4.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i1i5 = ipv4.Assign (d1d5);
    
 
    ipv4.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i4 = ipv4.Assign (d0d4);
    
    
    ipv4.SetBase ("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer i1i4 = ipv4.Assign (d1d4);
    

   // ipv4.SetBase ("10.1.5.0", "255.255.255.0");
   // Ipv4InterfaceContainer i3i5 = ipv4.Assign (d3d5);
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
   
   


//////// INTERFERENCE TRAFFIC SETUP ///////////////////

   // Create the OnOff application to send UDP datagrams of size
   // 210 bytes at a rate of 448 Kb/s from n1 to n2
   //NS_LOG_INFO ("Create Applications.");
   uint16_t port = 9;
   OnOffHelper onoff ("ns3::UdpSocketFactory", Address (InetSocketAddress (i1i4.GetAddress (0), port)));
   onoff.SetConstantRate (DataRate ("448kb/s"));
   ApplicationContainer apps = onoff.Install (csmaNodes.Get (1));
   apps.Start (Seconds (1.0));
   apps.Stop (Seconds (10.0));

///////////////////////////////////////////////////////////


///// INTERFACE TO VIRTUAL MACHINES SETUP USING TAP-BRIDGE ///////////

   // Use the TapBridgeHelper to connect to the pre-configured tap devices for
    // the left side.  We go with "UseBridge" mode since the CSMA devices support
    // promiscuous mode and can therefore make it appear that the bridge is 
    // extended into ns-3.  The install method essentially bridges the specified
    // tap to the specified CSMA device.
    //
    TapBridgeHelper tapBridge;
    tapBridge.SetAttribute ("Mode", StringValue ("UseBridge"));
    tapBridge.SetAttribute ("DeviceName", StringValue ("tap-left"));
    tapBridge.Install (csmaNodes.Get (0), d0d5.Get (0));
  
    //
    // Connect the right side tap to the right side CSMA device on the right-side
    // ghost node.
    //
    tapBridge.SetAttribute ("DeviceName", StringValue ("tap-right"));
    tapBridge.Install (csmaNodes.Get (1), d0d5.Get (1));
   
////////////////////////////////////////////////////////////////////////
   
  
    
    // MONITORAGGIO DI FLUSSO         
  //  Ptr<FlowMonitor> flowMonitor; 
  //  FlowMonitorHelper flowmonHelper;
  //  flowmonHelper.InstallAll ();
	
   // MOBILITY
    MobilityHelper mobility;
    mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                "X", StringValue ("100.0"),
                                "Y", StringValue ("100.0"),
                                "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");    
    mobility.Install (csmaNodes);

//    AnimationInterface anim ("csma.xml");

	
//    AsciiTraceHelper ascii;
//    csma.EnableAsciiAll (ascii.CreateFileStream ("simple-global-routing.tr"));
//    csma.EnablePcapAll ("simple-global-routing");
//    csma.EnablePcapAll ("tap-csma", false);

   //
   // Run the simulation for ten minutes to give the user time to play around
   //
   Simulator::Stop (Seconds (600));
   Simulator::Run ();
   Simulator::Destroy ();
 }
