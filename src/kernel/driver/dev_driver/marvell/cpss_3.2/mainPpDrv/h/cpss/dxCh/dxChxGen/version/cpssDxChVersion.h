/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChVersion.h
*
* DESCRIPTION:
*       Includes software CPSS DxCh version information.
*
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#ifndef __cpssDxChVersionh
#define __cpssDxChVersionh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/version/cpssGenVersion.h>

/*******************************************************************************
* cpssDxChVersionGet
*
* DESCRIPTION:
*       This function returns CPSS DxCh version.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       versionPtr     - (pointer to)CPSS DxCh version info.
*
* RETURNS:
*       GT_OK on success,
*       GT_BAD_PARAM on bad parameters,
*       GT_BAD_SIZE - the version name is too long
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVersionGet
(
    OUT CPSS_VERSION_INFO_STC   *versionPtr
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChVersionh */

