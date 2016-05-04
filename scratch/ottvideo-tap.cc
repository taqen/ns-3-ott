/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include <iostream>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/tap-bridge-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TapCsmaExample");

int 
main (int argc, char *argv[])
{
  std::string mode = "UseBridge";
  uint32_t simTime = 60.0;
  uint32_t dataRate = 5000000;
  double delay = 2.0; //in ms
  CommandLine cmd;
  cmd.AddValue ("simTime", "SimulationTime", simTime);
  cmd.AddValue ("dataRate", "data rate in b/s", dataRate);
  cmd.AddValue ("delay", "delay in ms", delay);
  cmd.Parse (argc, argv);

  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));

  NodeContainer nodes;
  nodes.Create (2);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (dataRate));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (delay)));

  NetDeviceContainer devices = csma.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper addresses;
  addresses.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = addresses.Assign (devices);

  TapBridgeHelper tapBridge;
  tapBridge.SetAttribute ("Mode", StringValue (mode));
  tapBridge.SetAttribute ("DeviceName", StringValue("tap-left"));
  tapBridge.Install (nodes.Get (0), devices.Get (0));
  tapBridge.SetAttribute ("DeviceName", StringValue("tap-right"));
  tapBridge.Install (nodes.Get (1), devices.Get(1));

  csma.EnablePcapAll ("ottvide-tap", false);
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  Simulator::Stop (Seconds (simTime));
  Simulator::Run ();

  flowMonitor->SerializeToXmlFile("ottvideo-tap.xml", true, true);

  Simulator::Destroy ();

}
