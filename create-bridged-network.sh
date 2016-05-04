#!/bin/bash
brctl addbr br-left
brctl addbr br-right
tunctl -t tap-left
tunctl -t tap-right
ifconfig tap-left 0.0.0.0 promisc up
ifconfig tap-right 0.0.0.0 promisc up
brctl addif br-left tap-left
ifconfig br-left up
brctl addif br-right tap-right
ifconfig br-right up
modprobe br_netfilter
pushd /proc/sys/net/bridge
for f in bridge-nf-*; do echo 0 > $f; done
popd
