/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssMath.h
*
* DESCRIPTION:
*       Math operations for CPSS.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvCpssMathh
#define __prvCpssMathh

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* prvCpssMathAdd64
*
* DESCRIPTION:
*       Summarize two 64 bits values.
*
* INPUTS:
*       x - first value for sum.
*       y - second value for sum
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Calculated sum.
*
* COMMENTS:
*       This function does not care 65 bit overflow.
*
*******************************************************************************/
GT_U64  prvCpssMathAdd64
(
    IN  GT_U64 x,
    IN  GT_U64 y
);

/*******************************************************************************
* prvCpssMathIsPowerOf2
*
* DESCRIPTION:
*       This routine checks whether number is a power of 2.
*
* INPUTS:
*       number - the number to verify.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE  - for true.
*       GT_FALSE - for false.
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL prvCpssMathIsPowerOf2
(
    IN GT_U32 number
);

/*******************************************************************************
* prvCpssMathLog2
*
* DESCRIPTION:
*       This function calculates log(x) by base 2.
*
* INPUTS:
*       num - The number to perform the calculation on, This number must be a
*             power of 2.
*
* OUTPUTS:
*       resultPtr  - pointer to log(num) result.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise (if the number is not a power of 2).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssMathLog2
(
    IN  GT_U32  num,
    OUT GT_U32  *resultPtr
);

/*******************************************************************************
* prvCpssRoundDiv
*
* DESCRIPTION:
*   Divide and round a value with accuracy of 3 digits.
*
* INPUTS:
*   value   - Value to divide.
*   divisor - the divisor.
*
* OUTPUTS:
*   resultPtr   - pointer to result after round
*
* RETURNS :
*   GT_OK
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssRoundDiv
(
    IN  GT_U32  value,
    IN  GT_U32  divisor,
    OUT GT_U32 *resultPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssMathh */

