/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
********************************************************************************
* appDemoFdbUpdateLock.h
*
* DESCRIPTION:
*       App demo lock for Fdb update actions
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __appDemoFdbUpdateLockh
#define __appDemoFdbUpdateLockh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* A mutual exclusion required with the following CPSS APIs:
*   - cpssDxChBrgFdbMacEntrySet
*   - cpssDxChBrgFdbQaSend
*   - cpssDxChBrgFdbMacEntryDelete
*   - cpssDxChBrgFdbMacEntryWrite
*   - cpssDxChBrgFdbMacEntryRead
*   - cpssDxChBrgFdbMacEntryInvalidate
*   - cpssDxChBrgFdbFromCpuAuMsgStatusGet
*   - cpssDxChBrgFdbAuMsgBlockGet
*   - cpssDxChBrgFdbFuMsgBlockGet
*   - cpssDxChBrgFdbAuqFuqMessagesNumberGet
*
*   - cpssDxChBrgFdbActionsEnableSet
*   - cpssDxChBrgFdbTrigActionStatusGet
*   - cpssDxChBrgFdbMacTriggerModeSet
*   - cpssDxChBrgFdbTrigActionStart
*   - cpssDxChBrgFdbMacTriggerToggle
*
* Pay attention that pairs of calls must be treated as one call from mutual
* exclusion point of view:
*   LOCK
*   cpssDxChBrgFdbMacEntrySet
*   while(..)
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet;
*   UNLOCK
*
*   LOCK
*   cpssDxChBrgFdbMacEntryDelete
*   while(..)
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet;
*   UNLOCK
*
*   LOCK
*   cpssDxChBrgFdbQaSend
*   while(..)
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet;
*   UNLOCK
*******************************************************************************/
#ifndef INCLUDE_UTF
#  define APPDEMO_FDB_UPDATE_LOCK()
#  define APPDEMO_FDB_UPDATE_UNLOCK()
#else
#  include <cpss/extServices/private/prvCpssBindFunc.h>
   extern CPSS_OS_MUTEX appDemoFdbUpdateLockMtx;
#  define APPDEMO_FDB_UPDATE_LOCK() cpssOsMutexLock(appDemoFdbUpdateLockMtx)
#  define APPDEMO_FDB_UPDATE_UNLOCK() cpssOsMutexUnlock(appDemoFdbUpdateLockMtx)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __appDemoFdbUpdateLockh */
