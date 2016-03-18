/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 35929 $
 * $Date: 2013-01-08 11:39:47 +0800 (Tue, 08 Jan 2013) $
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : Atomic relative API
 *
 */

#ifndef __OSAL_ATOMIC_H__
#define __OSAL_ATOMIC_H__

/*
 * Include Files
 */
#include <common/type.h>
#include <linux/sched.h>

/*
 * Symbol Definition
 */
#define osal_atomic_t  atomic_t

/*
 * Function Declaration
 */

/* Function Name:
 *      osal_atomic_inc
 * Description:
 *      Increase the atomic counter.
 * Input:
 *      None
 * Output:
 *      pAtomic - pointer buffer of atomic counter.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32
osal_atomic_inc(osal_atomic_t *pAtomic);

/* Function Name:
 *      osal_atomic_set
 * Description:
 *      Set the atomic counter.
 * Input:
 *      counter - atomic counter to be configured
 * Output:
 *      pAtomic - pointer buffer of atomic counter.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32
osal_atomic_set(osal_atomic_t *pAtomic, int32 counter);

/* Function Name:
 *      osal_atomic_dec_return
 * Description:
 *      Atomically increments atomic counter by 1 and returns the result.
 * Input:
 *      pAtomic - pointer buffer of atomic counter.
 * Output:
 *      pAtomic - pointer buffer of atomic counter.
 * Return:
 *      returns the result of atomic counter
 * Note:
 *      None
 */
extern int32
osal_atomic_dec_return(osal_atomic_t *pAtomic);

/* Function Name:
 *      osal_atomic_read
 * Description:
 *      Atomically reads the atomic counter.
 * Input:
 *      pAtomic - pointer buffer of atomic counter.
 * Output:
 *      None
 * Return:
 *      returns the read value of atomic counter
 * Note:
 *      None
 */
extern int32
osal_atomic_read(osal_atomic_t *pAtomic);

#endif /* __OSAL_ATOMIC_H__ */
