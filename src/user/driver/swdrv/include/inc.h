#ifndef __INC_H__
#define __INC_H__

#include "sys_adpt.h"
#include <bcm/types.h>
#include "pbmp.h"


/**************************************************

 bcm/types.h

 **************************************************/
#define BCM_PBMP_CLEAR(pbm)     _SHR_PBMP_CLEAR(pbm)
#define BCM_PBMP_MEMBER(bmp, port)  _SHR_PBMP_MEMBER((bmp), (port))
#define BCM_PBMP_ITER(bmp, port)  _SHR_PBMP_ITER((bmp), (port))
#define BCM_PBMP_COUNT(pbm, count)  _SHR_PBMP_COUNT(pbm, count)

#define BCM_PBMP_IS_NULL(pbm)   _SHR_PBMP_IS_NULL(pbm)
#define BCM_PBMP_NOT_NULL(pbm)  _SHR_PBMP_NOT_NULL(pbm)
#define BCM_PBMP_EQ(pbm_a, pbm_b)  _SHR_PBMP_EQ(pbm_a, pbm_b)
#define BCM_PBMP_NEQ(pbm_a, pbm_b)  _SHR_PBMP_NEQ(pbm_a, pbm_b)

/* Assignment operators */
#define BCM_PBMP_ASSIGN(dst, src)  _SHR_PBMP_ASSIGN(dst, src)
#define BCM_PBMP_AND(pbm_a, pbm_b)  _SHR_PBMP_AND(pbm_a, pbm_b)
#define BCM_PBMP_OR(pbm_a, pbm_b)  _SHR_PBMP_OR(pbm_a, pbm_b)
#define BCM_PBMP_XOR(pbm_a, pbm_b)  _SHR_PBMP_XOR(pbm_a, pbm_b)
#define BCM_PBMP_REMOVE(pbm_a, pbm_b)  _SHR_PBMP_REMOVE(pbm_a, pbm_b)
#define BCM_PBMP_NEGATE(pbm_a, pbm_b)  _SHR_PBMP_NEGATE(pbm_a, pbm_b)

/* Port PBMP operators */
#define BCM_PBMP_PORT_SET(pbm, port)  _SHR_PBMP_PORT_SET(pbm, port)
#define BCM_PBMP_PORT_ADD(pbm, port)  _SHR_PBMP_PORT_ADD(pbm, port)
#define BCM_PBMP_PORT_REMOVE(pbm, port)  _SHR_PBMP_PORT_REMOVE(pbm, port)
#define BCM_PBMP_PORT_FLIP(pbm, port)  _SHR_PBMP_PORT_FLIP(pbm, port)

/* Set the default tag protocol ID (TPID) for the specified port. */
//typedef int bcm_port_t;

/* bcm_pbmp_t */
//typedef _shr_pbmp_t bcm_pbmp_t;




/**************************************************

 dev_rm.h

 **************************************************/


#define BCM_FIELD_USER_FIELD_SIZE   4


#endif // __INC_H__
