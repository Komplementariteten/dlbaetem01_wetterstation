# 1. Init
target extended-remote /dev/serial/by-id/usb-Black_Magic_Debug_Black_Magic_Probe_v2.0.0_72AE49F4-if00

# 2.
monitor swdp_scan
attach 1
set mem inaccessible-by-default off
load
compare-sections
start
run