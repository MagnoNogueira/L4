/* MODULE NAME:  l_HostString2Ip.c
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
*
* Copyright(C)      Accton Corporation, 2003
*/



/* INCLUDE FILE DECLARATIONS
 */
#include <string.h>
#include <stdlib.h>
#include "sys_type.h"
#include "sys_cpnt.h"
#include "l_hoststring2ip.h"

#if( SYS_CPNT_DNS == TRUE )
#include "dns_mgr.h"

#endif

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
enum
{
    ATOIP_OK = 0,
    ATOIP_INVALID_IP,
    ATOIP_NON_IP_STRING,
};

/* LOCAL SUBPROGRAM DECLARATIONS
 */
/* FUNCTION NAME:  AtoIp
 * PURPOSE:
 *			Convert string s (xxx.xxx.xxx.xxx) to IP address (4-byte object).
 *
 * INPUT:
 *          UI8_T   *hostip_p   --  host ip.
 *
 * OUTPUT:
 *          UI8_T   *ip_p       --  ip address.
 *
 * RETURN:
 *          TRUE to indicate successful and FALSE to indicate failure.
 * NOTES:
 *          This function is invoked in L_HostString2Ip().
 */
static UI32_T AtoIp(UI8_T *hostname_p, UI8_T *ip_p);



/* STATIC VARIABLE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM BODIES
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
UI32_T L_HostString2IP(UI8_T *hoststr_p, UI32_T *ip_p)
{
    /* LOCAL CONSTANT DECLARATIONS
    */

    /* LOCAL VARIABLES DECLARATIONS
    */
    UI32_T  ip_addr = 0;
    UI32_T  ret = 0;

    /* BODY */
    ret = AtoIp(hoststr_p, (UI8_T*)&ip_addr);
    if( ret == ATOIP_OK )
    {
        *ip_p = ip_addr;
        return HOSTSTRING2IP_OK;
    }
    else if( ret == ATOIP_INVALID_IP )
    {
        *ip_p = 0;
        return HOSTSTRING2IP_INVALID_IP;
    }
    else
    {
#if( SYS_CPNT_DNS == TRUE )
        /*ret = DNS_MGR_HostNameToIp(hoststr_p, &ip_addr);  */
        ret = DNS_MGR_HostNameToIp(hoststr_p, ip_p);
        if( ret == 3/* DNS_RC_NAMEERR */ )
        {
            *ip_p = 0;
            return HOSTSTRING2IP_NON_EXISTENT_DOMAIIN;
        }
        else if( ret == 2/* DNS_RC_SERVFAIL */ )
        {
            *ip_p = 0;
            return HOSTSTRING2IP_NO_RESPONSE_FROM_SERVER;
        }
        else if( ret == 0/* DNS_RC_OK */ )
        {
            /* *ip_p = ip_addr; */
            return HOSTSTRING2IP_OK;
        }
        else if( ret == 300 )
        {
            *ip_p = 0;
            return HOSTSTRING2IP_DNS_REQUEST_TIMED_OUT;
        }
        else
        {
            *ip_p = 0;
            return HOSTSTRING2IP_NO_RESPONSE_FROM_SERVER;
        }
#else
        *ip_p = 0;
        return HOSTSTRING2IP_INVALID_IP;
#endif
    }
}







/* LOCAL SUBPROGRAM BODIES
 */

/* FUNCTION NAME:  AtoIp
 * PURPOSE:
 *			Convert string (xxx.xxx.xxx.xxx) to IP address (4-byte object).
 *
 * INPUT:
 *          UI8_T   *hostip_p   --  host ip.
 *
 * OUTPUT:
 *          UI8_T   *ip_p       --  ip address.
 *
 * RETURN:
 *          TRUE to indicate successful and FALSE to indicate failure.
 * NOTES:
 *          This function is invoked in L_HostString2Ip().
 */
static UI32_T AtoIp(UI8_T *hostip_p, UI8_T *ip_p)
{
	/* LOCAL CONSTANT DECLARATIONS
	*/

	/* LOCAL VARIABLES DECLARATIONS
	*/
    UI8_T token[20];
    int   i,j;  /* i for s[]; j for token[] */
    int   k;    /* k for ip[] */
    UI8_T temp[4];

	/* BODY */
    i = 0;
    j = 0;
    k = 0;

    while (hostip_p[i] != '\0')
    {
        if (hostip_p[i] == '.')
        {
            token[j] = '\0';
            if (strlen((char *)token) < 1 || strlen((char *)token) > 3 ||
                atoi((char *)token) < 0 || atoi((char *)token) > 255)
            {
                return ATOIP_INVALID_IP;
            }
            else if (k >= 4)
            {
                return ATOIP_INVALID_IP;
            }
            else
            {
                temp[k++] =(UI8_T)atoi((char *)token);
                i++; j = 0;
            }
        }
        else if (!(hostip_p[i] >= '0' && hostip_p[i] <= '9'))
        {
            return ATOIP_NON_IP_STRING;
        }
        else
        {
            token[j++] = hostip_p[i++];
        }

    } /* while */

    token[j] = '\0';
    if (strlen((char *)token) < 1 || strlen((char *)token) > 3 ||
        atoi((char *)token) < 0 || atoi((char *)token) > 255)
    {
        return ATOIP_INVALID_IP;
    }
    else if (k != 3)
    {
        return ATOIP_INVALID_IP;
    }

    temp[k]=(UI8_T)atoi((char *)token);

    ip_p[0] = temp[0];
    ip_p[1] = temp[1];
    ip_p[2] = temp[2];
    ip_p[3] = temp[3];

    return ATOIP_OK;
} /* End of AtoIp() */








