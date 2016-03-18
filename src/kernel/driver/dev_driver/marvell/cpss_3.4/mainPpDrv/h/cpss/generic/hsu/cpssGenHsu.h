/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenHsu.h
*
* DESCRIPTION:
*       Includes generic HSU definitions.
*
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

#ifndef __cpssGenHsuh
#define __cpssGenHsuh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>

/* indicates single iteration export/import */
#define CPSS_HSU_SINGLE_ITERATION_CNS                       0xFFFFFFFF

/*
 * typedef: enum CPSS_HSU_DATA_TYPE_ENT
 *
 * Description:
 *      Different data types that can be exported and imported.
 *
 * Enumerations:
 *
 *    CPSS_HSU_DATA_TYPE_TCAM_MANAGER_E       - TCAM manager HSU data type
 */

typedef enum
{
    CPSS_HSU_DATA_TYPE_TCAM_MANAGER_E

} CPSS_HSU_DATA_TYPE_ENT;

/*******************************************************************************
* cpssHsuEventHandleUpdate
*
* DESCRIPTION:
*       This function replace old event  handle created after regular init
*       to new event handle that application got during HSU.
*
* APPLICABLE DEVICES: ALL
*
* INPUTS:
*       oldEvReqHndl               - old event  handle created after regular init.
*       newEvReqHndl               - new event handle  created during HSU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssHsuEventHandleUpdate
(
    IN     GT_U32 oldEvReqHndl,
    IN    GT_U32  newEvReqHndl
);

/*******************************************************************************
* cpssHsuStateSet
*
* DESCRIPTION:
*       This function inform cpss about HSU state
*
* APPLICABLE DEVICES: ALL
*
* INPUTS:
*       hsuInfoPtr               - pointer to hsu information
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                  - on success.
*       GT_BAD_PTR             - on NULL pointer
*       GT_BAD_PARAM           - on bad hsuState
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssHsuStateSet
(
    IN CPSS_HSU_INFO_STC    *hsuInfoPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenHsuh */

