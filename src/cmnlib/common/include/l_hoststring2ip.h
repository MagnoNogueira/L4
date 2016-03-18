/* MODULE NAME:  l_HostString2Ip.h
* PURPOSE:
*           This package provides a function to covert a host string
*           (domain name or decimal dot ip string)
*           to network-format IPv4 address.
*
* NOTES:
*
* History:
*       Date          -- Modifier,  Reason
*     2003-02-11      -- Isiah , created.
*     2003-02-14      -- JJYoung, spec. modified.
*
* Copyright(C)      Accton Corporation, 2003
*/

#ifndef L_HOSTSTRING2IP_H

#define L_HOSTSTRING2IP_H



/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */
#define L_HostString2Ip(a, b)   L_HostString2IP((a), (b))

/* DATA TYPE DECLARATIONS
 */
enum
{
    HOSTSTRING2IP_OK = 0,
    HOSTSTRING2IP_INVALID_IP,
    HOSTSTRING2IP_NON_EXISTENT_DOMAIIN,
    HOSTSTRING2IP_DNS_REQUEST_TIMED_OUT,
    HOSTSTRING2IP_NO_RESPONSE_FROM_SERVER,
};

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/* FUNCTION NAME:  L_HostString2IP
 * PURPOSE:
 *          This function is to covert a host string
 *          (domain name or decimal dot ipdot string)
 *          to network-format internet IPv4 address.
 *
 * INPUT:
 *          UI8_T   *hoststr_p  --  host string
 *                                  (domain name or decimal dot ip string)
 *          UI32_T  *ip_p       --  ip address (network-format), used when *hoststr_p is domain name
 *                                  input 0 is to get the first one in host table. if no entry in host table -> output *ip_p=0
 *                                  input !=0 is to get the next of the given ip; however if can't match given IP in host table -> output *ip_p=0
 *
 * OUTPUT:
 *          UI32_T  *ip_p       --  ip address (network-format)
 *
 * RETURN:
 *          0 -- success
 *          1 -- invalid ip
 *          2 -- Non-existent domain
 *          3 -- DNS request timed out
 *          4 -- No response from server
 *
 * NOTES:
 *
 */
UI32_T L_HostString2IP(UI8_T *hoststr_p, UI32_T *ip_p);

#endif /* #ifndef L_HOSTSTRING2IP_H */



