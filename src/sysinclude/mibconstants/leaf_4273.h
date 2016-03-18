#ifndef LEAF_4273_H
#define LEAF_4273_H

/* bgpVersion
 */
#define LEAF_bgpVersion   1
#define MINSIZE_bgpVersion  1L
#define MAXSIZE_bgpVersion  255L

/* bgpLocalAs
 */
#define LEAF_bgpLocalAs   2
#define MIN_bgpLocalAs  0L
#define MAX_bgpLocalAs  65535L

/* bgpPeerTable
 */
#define LEAF_bgpPeerIdentifier   1
#define LEAF_bgpPeerState   2
#define VAL_bgpPeerState_idle 1L
#define VAL_bgpPeerState_connect 2L
#define VAL_bgpPeerState_active 3L
#define VAL_bgpPeerState_opensent 4L
#define VAL_bgpPeerState_openconfirm 5L
#define VAL_bgpPeerState_established 6L
#define LEAF_bgpPeerAdminStatus   3
#define VAL_bgpPeerAdminStatus_stop 1L
#define VAL_bgpPeerAdminStatus_start 2L
#define LEAF_bgpPeerNegotiatedVersion   4
#define LEAF_bgpPeerLocalAddr   5
#define LEAF_bgpPeerLocalPort   6
#define MIN_bgpPeerLocalPort  0L
#define MAX_bgpPeerLocalPort  65535L
#define LEAF_bgpPeerRemoteAddr   7
#define LEAF_bgpPeerRemotePort   8
#define MIN_bgpPeerRemotePort  0L
#define MAX_bgpPeerRemotePort  65535L
#define LEAF_bgpPeerRemoteAs   9
#define MIN_bgpPeerRemoteAs  0L
#define MAX_bgpPeerRemoteAs  65535L
#define LEAF_bgpPeerInUpdates   10
#define LEAF_bgpPeerOutUpdates   11
#define LEAF_bgpPeerInTotalMessages   12
#define LEAF_bgpPeerOutTotalMessages   13
#define LEAF_bgpPeerLastError   14
#define SIZE_bgpPeerLastError 2L
#define LEAF_bgpPeerFsmEstablishedTransitions   15
#define LEAF_bgpPeerFsmEstablishedTime   16
#define LEAF_bgpPeerConnectRetryInterval   17
#define MIN_bgpPeerConnectRetryInterval  1L
#define MAX_bgpPeerConnectRetryInterval  65535L
#define LEAF_bgpPeerHoldTime   18
#define MIN_bgpPeerHoldTime  3L
#define MAX_bgpPeerHoldTime  65535L
#define LEAF_bgpPeerKeepAlive   19
#define MIN_bgpPeerKeepAlive  1L
#define MAX_bgpPeerKeepAlive  21845L
#define LEAF_bgpPeerHoldTimeConfigured   20
#define MIN_bgpPeerHoldTimeConfigured  3L
#define MAX_bgpPeerHoldTimeConfigured  65535L
#define LEAF_bgpPeerKeepAliveConfigured   21
#define MIN_bgpPeerKeepAliveConfigured  1L
#define MAX_bgpPeerKeepAliveConfigured  21845L
#define LEAF_bgpPeerMinASOriginationInterval   22
#define MIN_bgpPeerMinASOriginationInterval  1L
#define MAX_bgpPeerMinASOriginationInterval  65535L
#define LEAF_bgpPeerMinRouteAdvertisementInterval   23
#define MIN_bgpPeerMinRouteAdvertisementInterval  1L
#define MAX_bgpPeerMinRouteAdvertisementInterval  65535L
#define LEAF_bgpPeerInUpdateElapsedTime   24

/* bgpIdentifier function declarations
 */
#define LEAF_bgpIdentifier   4

/* bgpRcvdPathAttrTable function declarations
 */
#define LEAF_bgpPathAttrPeer   1
#define LEAF_bgpPathAttrDestNetwork   2
#define LEAF_bgpPathAttrOrigin   3
#define VAL_bgpPathAttrOrigin_igp 1L
#define VAL_bgpPathAttrOrigin_egp 2L
#define VAL_bgpPathAttrOrigin_incomplete 3L
#define LEAF_bgpPathAttrASPath   4
#define MINSIZE_bgpPathAttrASPath  2L
#define MAXSIZE_bgpPathAttrASPath  255L
#define LEAF_bgpPathAttrNextHop   5
#define LEAF_bgpPathAttrInterASMetric   6

/* bgp4PathAttrTable
 */
#define LEAF_bgp4PathAttrPeer   1
#define LEAF_bgp4PathAttrIpAddrPrefixLen   2
#define MIN_bgp4PathAttrIpAddrPrefixLen  0L
#define MAX_bgp4PathAttrIpAddrPrefixLen  32L
#define LEAF_bgp4PathAttrIpAddrPrefix   3
#define LEAF_bgp4PathAttrOrigin   4
#define VAL_bgp4PathAttrOrigin_igp 1L
#define VAL_bgp4PathAttrOrigin_egp 2L
#define VAL_bgp4PathAttrOrigin_incomplete 3L
#define LEAF_bgp4PathAttrASPathSegment   5
#define MINSIZE_bgp4PathAttrASPathSegment  2L
#define MAXSIZE_bgp4PathAttrASPathSegment  255L
#define LEAF_bgp4PathAttrNextHop   6
#define LEAF_bgp4PathAttrMultiExitDisc   7
#define MIN_bgp4PathAttrMultiExitDisc  -1L
#define MAX_bgp4PathAttrMultiExitDisc  2147483647L
#define LEAF_bgp4PathAttrLocalPref   8
#define MIN_bgp4PathAttrLocalPref  -1L
#define MAX_bgp4PathAttrLocalPref  2147483647L
#define LEAF_bgp4PathAttrAtomicAggregate   9
#define VAL_bgp4PathAttrAtomicAggregate_lessSpecificRouteNotSelected 1L
#define VAL_bgp4PathAttrAtomicAggregate_lessSpecificRouteSelected 2L
#define LEAF_bgp4PathAttrAggregatorAS   10
#define MIN_bgp4PathAttrAggregatorAS  0L
#define MAX_bgp4PathAttrAggregatorAS  65535L
#define LEAF_bgp4PathAttrAggregatorAddr   11
#define LEAF_bgp4PathAttrCalcLocalPref   12
#define MIN_bgp4PathAttrCalcLocalPref  -1L
#define MAX_bgp4PathAttrCalcLocalPref  2147483647L
#define LEAF_bgp4PathAttrBest   13
#define VAL_bgp4PathAttrBest_false 1L
#define VAL_bgp4PathAttrBest_true 2L
#define LEAF_bgp4PathAttrUnknown   14
#define MINSIZE_bgp4PathAttrUnknown  0L
#define MAXSIZE_bgp4PathAttrUnknown  255L

#endif /* LEAF_4273_H */

