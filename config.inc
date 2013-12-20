# Mandatory: Select your BSP and installation prefix
TARGET = arm-rtems4.11
BSP = realview_pbx_a9_qemu
PREFIX = /opt/rtems-4.11

# Optional: Separate installation base directory
INSTALL_BASE = $(PREFIX)/$(TARGET)/$(BSP)

# Optional: Network test configuration
TEST_RUNNER = $(BSP)
NET_CFG_SELF_IP = 10.0.2.1
NET_CFG_NETMASK = 255.255.0.0
NET_CFG_PEER_IP = 192.168.100.11
NET_CFG_GATEWAY_IP = 192.168.100.11
NET_TAP_INTERFACE = tap0
