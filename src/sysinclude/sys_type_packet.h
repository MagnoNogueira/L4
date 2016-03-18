/* ------------------------------------------------------------------------
 * FILE NAME - SYS_TYPE_PACKET.H                                                     
 * ------------------------------------------------------------------------
 * ABSTRACT :                                                              
 * Purpose: This package defines the naming constants for various packet
 *          headers.
 * Note: 1. Most of the numbers are assigned by IANA. From RFC3232, it
 *          provides a WEB site 'www.iana.org' to keep the latest
 *          assigned numbers.
 *       2. This package only defines the well-known numbers and which are
 *          used in our system.
 *       3. This package shall be only included by sys_type.h
 * ------------------------------------------------------------------------
 *  History
 *
 *   TimLu     06/22/2005      new created.
 *   TimLu     06/23/2005      Add constants for L2, multicast address,
 *                             and IPv6.
 *   TimLu     06/27/2005      Add 0x9100 for 802.1Q Tunneling.
 *   TimLu     07/08/2005      Add LLDP EtherType 0x088CC,
 *                             and LLDP Multicast Address 01-80-C2-00-00-0E
 * ------------------------------------------------------------------------
 * Copyright(C)							  	ACCTON Technology Corp. , 2005      
 * ------------------------------------------------------------------------
 */
 
#ifndef SYS_TYPE_PACKET_H
#define SYS_TYPE_PACKET_H

/* NAMING CONSTANT DECLARATIONS 
 */

/* 1. The followings define the numbers for EtherType in Ethernet header,
 *    excluding 'SYS_TYPE_PACKET_ETHERNET_HEADER_TYPE_FOR_LENGTH'.
 * 2. The latest definition can be found in the 'Ethernet Numbers' of
 *    'www.iana.org' web page.
 * 3. The Encapsulation of IP datagrams is defined in RFC 894 for Ethernet
 *    and in RFC1042 for IEEE802 networks. If the EtherType field number is
 *    greater than 1500 (0x05DC), then this field indicates the type
 *    of IP datagram; if the EtherType field number is not greater than 1500,
 *    than this field indicates the length of following the EtherType field.
 * 4. The well-known numbers and which are used in our system for EtherType:
 *        0x0800 means IP packet
 *        0x0806 means ARP packet
 *        0x0835 means RARP packet
 *        0x86DD means IPv6 packet
 *        0x8809 means LACP packet
 *        0x888E means EAPOL packet
 *        0x88CC means LLDP packet
 * 5. EAPOL (EAP over LAN) is defined in IEEE 802.1x, it uses the 
 *    'Port Access Entity Ethernet Type' 0x888E for the EtherType.
 * 6. LACP is defined in IEEE 802.3ad with destination mac address
 *    01-80-C2-00-00-02. LACP is a general term for link aggregation
 *    , it covers Marker Protocol.
 * 7. VLAN is defined in IEEE 802.1Q. For VLAN, there are 4 bytes inserted
 *    behind the original source MAC address. The 0x8100 is the VLAN
 *    protocol ID, and it is located as the original EtherType field.
 *    The 0x9100 is for Q in Q, that is double tag (or IEEE802.1Q Tunneling
 *    in Cisco's term).
 */
#define SYS_TYPE_PACKET_ETHERNET_HEADER_TYPE_FOR_IP                     0x0800
#define SYS_TYPE_PACKET_ETHERNET_HEADER_TYPE_FOR_ARP                    0x0806
#define SYS_TYPE_PACKET_ETHERNET_HEADER_TYPE_FOR_RARP                   0x0835
#define SYS_TYPE_PACKET_ETHERNET_HEADER_TYPE_FOR_VLAN                   0x8100
#define SYS_TYPE_PACKET_ETHERNET_HEADER_TYPE_FOR_IP6                    0x86DD
#define SYS_TYPE_PACKET_ETHERNET_HEADER_TYPE_FOR_LACP                   0x8809
#define SYS_TYPE_PACKET_ETHERNET_HEADER_TYPE_FOR_EAPOL                  0x888E
#define SYS_TYPE_PACKET_ETHERNET_HEADER_TYPE_FOR_LLDP                   0x88CC
#define SYS_TYPE_PACKET_ETHERNET_HEADER_TYPE_FOR_DOUBLE_TAG             0x9100
#define SYS_TYPE_PACKET_ETHERNET_HEADER_TYPE_FOR_LENGTH                 0x05DC

/* 1. The followings define the numbers for Protocol in IP header.
 * 2. The latest definition can be found in the 'Protocol Numbers' of
 *    'www.iana.org' web page.
 * 3. The details of IP datagram definition can be found in RFC791.
 * 4. The well-known numbers and which are used in our system for 
 *    Protocol in IP header:
 *        1 (0x01) means ICMP packet
 *        2 (0x02) means IGMP packet or DVMRP packet
 *        4 (0x04) means IP packet
 *        6 (0x06) means TCP packet
 *       17 (0x11) means UDP packet
 *       41 (0x29) means IPv6 packet
 *       89 (0x59) means OSPF packet
 *      103 (0x67) means PIM packet 
 *      112 (0x70) means VRRP packet
 * 5. DVMRP packets are encapsulated in IP datagrams, with an IP protocol
 *    number of 2, it's the same as IGMP packets.
 * 6. Protocol number 4 or 41 is used for tunnelling, which (IPv4 packet or
 *    IPv6 packet encapsulated in an IP packet. 
 */
#define SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_ICMP                     0x01
#define SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_IGMP                     0x02
#define SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_DVMRP                    0x02
#define SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_IP                       0x04
#define SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_TCP                      0x06
#define SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_UDP                      0x11
#define SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_IP6                      0x29
#define SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_OSPF                     0x59
#define SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_PIM                      0x67
#define SYS_TYPE_PACKET_IP_HEADER_PROTOCOL_FOR_VRRP                     0x70

/* 1. The followings define the numbers for Next Header in IPv6 header.
 * 2. The latest definition can be found in the 'Protocol Numbers' of
 *    'www.iana.org' web page.
 * 3. The details of IPv6 datagram definition can be found in RFC2460.
 * 4. The well-known numbers and which are used in our system for 
 *    Next Header in IP header:
 *        0 (0x00) means Hop-by-Hop Options header
 *        6 (0x06) means TCP header
 *       17 (0x11) means UDP packet
 *       41 (0x29) means Encapsulated IPv6 header
 *       43 (0x2B) means Routing header
 *       44 (0x2C) means Fragment header
 *       50 (0x32) means Encapsulating Security Payload header
 *       51 (0x33) means Authentication header
 *       58 (0x3A) means ICMPv6 header
 *       59 (0x3B) means Routing header
 *       60 (0x3C) means Routing header
 */
#define SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_HOP                  0x00
#define SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_TCP                  0x06
#define SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_UDP                  0x11
#define SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_ENCAPSULATED         0x29
#define SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_ROUTING              0x2B
#define SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_FRAGMENT             0x2C
#define SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_SECURITY             0x32
#define SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_AUTHENTICATION       0x33
#define SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_ICMP6                0x3A
#define SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_NO_NEXT              0x3B
#define SYS_TYPE_PACKET_IP6_HEADER_NEXT_HEADER_FOR_DESTINATION          0x3C


/* 1. The followings define the numbers for port number in UDP header.
 * 2. The latest definition can be found in the 'Port Numbers' of
 *    'www.iana.org' web page.
 * 3. The details of UDP datagram definition can be found in RFC768.
 * 4. The well-known numbers and which are used in our system for port 
 *    number in UDP header:
 *       53 (0x0035) means DNS port number
 *       67 (0x0043) means DHCP or BOOTP server port number
 *       68 (0x0044) means DHCP or BOOTP client port number
 *       69 (0x0045) means TFTP port number
 *      123 (0x007B) means SNTP or NTP port number 
 *      161 (0x00A1) means SNMP server port number 
 *      162 (0x00A2) means SNMP client port number 
 *      520 (0x0208) means RIP port number
 *     1812 (0x0714) means RADIUS port number
 *     1813 (0x0715) means RADIUS Accounting port number
 *     1985 (0x07C1) means HSRP port number
 * 5. SNTP and NTP both use port 123.
 * 6. The implementation of DHCP is supposed to cover BOOTP.
 */
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_DNS                         0x0035
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_BOOTP_SERVER                0x0043
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_DHCP_SERVER                 0x0043
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_BOOTP_CLIENT                0x0044
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_DHCP_CLIENT                 0x0044
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_SNTP                        0x007B
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_NTP                         0x007B
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_SNMP_SERVER                 0x00A1
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_SNMP_CLIENT                 0x00A2
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_RIP                         0x0208
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_RADIUS                      0x0714
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_RADIUS_ACCOUNTING           0x0715
#define SYS_TYPE_PACKET_UDP_HEADER_PORT_FOR_HSRP                        0x07C1



/* 1. The followings define the numbers for port number in TCP header.
 * 2. The latest definition can be found in the 'Port Numbers' of
 *    'www.iana.org' web page.
 * 3. The details of TCP datagram definition can be found in RFC793.
 * 4. The well-known numbers and which are used in our system for port 
 *    number in UDP header:
 *       23 (0x0017) means TELNET port number
 *       49 (0x0031) means TACACS port number
 *       80 (0x0050) means HTTP port number
 *      443 (0x01BB) means HTTPS port number
 */
#define SYS_TYPE_PACKET_TCP_HEADER_PORT_FOR_TELNET                      0x0017
#define SYS_TYPE_PACKET_TCP_HEADER_PORT_FOR_TACACS                      0x0031
#define SYS_TYPE_PACKET_TCP_HEADER_PORT_FOR_HTTP                        0x0050
#define SYS_TYPE_PACKET_TCP_HEADER_PORT_FOR_HTTPS                       0x0443

/* 1. The followings define the numbers for IP multicast destination addresses.
 * 2. The latest definition can be found in the 
 *    'Multicast Address (IPv4) Address Assignments/Allocation' of
 *    'www.iana.org' web page.
 * 3. The well-known numbers and which are used in our system for 
 *    IP multicast addresses:
 *       224.0.0.1  (0xE0000001) means for IGMP all systems.
 *       224.0.0.2  (0xE0000002) means for IGMP all routers or HSRP.
 *       224.0.0.4  (0xE0000004) means for DVMRP.
 *       224.0.0.5  (0xE0000005) means for OSPF all routers.
 *       224.0.0.6  (0xE0000006) means for OSPF designated routers.
 *       224.0.0.9  (0xE0000009) means for RIPv2.
 *       224.0.0.13 (0xE000000D) means for PIM.
 *       224.0.0.18 (0xE0000012) means for VRRP.
 * 4. For HSRP, IANA assigns 224.0.0.102, but in RFC2281 
 *    'Cisco Hot Standby Router Protocol (HSRP) it defines to be 224.0.0.2,
 *    by implementation, we use 224.0.0.2, following RFC2281.
 */
#define SYS_TYPE_PACKET_IP_MULTICAST_ADDRESS_FOR_IGMP_ALL_SYSTEMS        0xE0000001
#define SYS_TYPE_PACKET_IP_MULTICAST_ADDRESS_FOR_IGMP_ALL_ROUTERS        0xE0000002
#define SYS_TYPE_PACKET_IP_MULTICAST_ADDRESS_FOR_HSRP                    0xE0000002
#define SYS_TYPE_PACKET_IP_MULTICAST_ADDRESS_FOR_DVMRP                   0xE0000004
#define SYS_TYPE_PACKET_IP_MULTICAST_ADDRESS_FOR_OSPF_ALL_ROUTERS        0xE0000005
#define SYS_TYPE_PACKET_IP_MULTICAST_ADDRESS_FOR_OSPF_DESIGNATED_ROUTERS 0xE0000006
#define SYS_TYPE_PACKET_IP_MULTICAST_ADDRESS_FOR_RIPV2                   0xE0000009
#define SYS_TYPE_PACKET_IP_MULTICAST_ADDRESS_FOR_PIM                     0xE000000D
#define SYS_TYPE_PACKET_IP_MULTICAST_ADDRESS_FOR_VRRP                    0xE0000012

/* 1. The followings define the numbers for MAC multicast destination addresses.
 * 2. The well-known numbers and which are used in our system for 
 *    MAC multicast addresses:
 *       01-80-C2-00-00-00  (0x0180C2000000) means for Spanning Tree Protocol.
 *       01-80-C2-00-00-01  (0x0180C2000001) means for IEEE 802.3 Full Deplex PAUSE.
 *       01-80-C2-00-00-02  (0x0180C2000002) means for LACP (including Marker Protocol).
 *       01-80-C2-00-00-03  (0x0180C2000003) means for Port Access Entity Group Address.
 *       01-80-C2-00-00-0E  (0x0180C200000E) means for IEEE 802.1AB Link Layer Discovery Protocol
 *       01-80-C2-00-00-21  (0x0180C2000021) means for GVRP.
 * 3. EAPOL uses 'Port Access Entity Group Address' as its destinestion address.
 * 4. Decalare all the numbers with string type.
 */
#define SYS_TYPE_PACKET_MAC_MULTICAST_ADDRESS_FOR_STP                   "\x01\x80\xC2\x00\x00\x00"
#define SYS_TYPE_PACKET_MAC_MULTICAST_ADDRESS_FOR_PAUSE                 "\x01\x80\xC2\x00\x00\x01"
#define SYS_TYPE_PACKET_MAC_MULTICAST_ADDRESS_FOR_LACP                  "\x01\x80\xC2\x00\x00\x02"
#define SYS_TYPE_PACKET_MAC_MULTICAST_ADDRESS_FOR_EAPOL                 "\x01\x80\xC2\x00\x00\x03"
#define SYS_TYPE_PACKET_MAC_MULTICAST_ADDRESS_FOR_LLDP                  "\x01\x80\xC2\x00\x00\x0E"
#define SYS_TYPE_PACKET_MAC_MULTICAST_ADDRESS_FOR_GVRP                  "\x01\x80\xC2\x00\x00\x21"


/* The followings define the length of various headers and addresses
 */
#define SYS_TYPE_PACKET_LENGTH_OF_MAC_ADDRESS                           6
/* The following 4 macro are defined based on rfc4001 */
#define SYS_TYPE_PACKET_LENGTH_OF_IP_ADDRESS                            4
#define SYS_TYPE_PACKET_LENGTH_OF_IPZONE_ADDRESS                        8
#define SYS_TYPE_PACKET_LENGTH_OF_IP6_ADDRESS                           16
#define SYS_TYPE_PACKET_LENGTH_OF_IP6ZONE_ADDRESS                       20
#define SYS_TYPE_PACKET_LENGTH_OF_ETHERNET_HEADER                       14
#define SYS_TYPE_PACKET_LENGTH_OF_IP_HEADER                             20
#define SYS_TYPE_PACKET_LENGTH_OF_IP6_HEADER                            40
#define SYS_TYPE_PACKET_LENGTH_OF_UDP_HEADER                            8
#define SYS_TYPE_PACKET_LENGTH_OF_TCP_HEADER                            20



/* MACRO DECLARATIONS
 */
 
/* IS_IP_BROADCAST_ADDRESS is for checking the IP address x is a broadcast.
 */
#define IS_IP_BROADCAST_ADDRESS(x)              (x == 0xFFFFFFFF)
            
          
#endif /* End of SYS_TYPE_PACKET_H */

