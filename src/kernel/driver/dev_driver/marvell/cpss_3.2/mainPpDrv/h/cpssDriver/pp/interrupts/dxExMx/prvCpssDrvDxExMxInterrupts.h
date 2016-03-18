/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvDxExMxInterrupts.h
*
* DESCRIPTION:
*       Includes general definitions for the interrupts handling unit.
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/
#ifndef __prvCpssDrvDxExMxInterrupts
#define __prvCpssDrvDxExMxInterrupts

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>

/*******************************************************************************
* prvCpssDrvDxExMxGppIsrConnect
*
* DESCRIPTION:
*       This function connects an Isr for a given Gpp interrupt.
*
* INPUTS:
*       devNum      - The Pp device number at which the Gpp device is connected.
*       gppId       - The Gpp Id to be connected.
*       isrFuncPtr  - A pointer to the function to be called on Gpp interrupt
*                     reception.
*       cookie      - A cookie to be passed to the isrFuncPtr when its called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       1.  To disconnect a Gpp Isr, call this function with a NULL parameter in
*           the isrFuncPtr param.
*
*******************************************************************************/
GT_STATUS prvCpssDrvDxExMxGppIsrConnect
(
    IN  GT_U8           devNum,
    IN  CPSS_EVENT_GPP_ID_ENT       gppId,
    IN  CPSS_EVENT_ISR_FUNC    isrFuncPtr,
    IN  void            *cookie
);

/*******************************************************************************
* prvCpssDrvDxExMxGppIsrCall
*
* DESCRIPTION:
*       Call the isr connected to a specified gpp.
*
* INPUTS:
*       devNum  - The Pp device number.
*       gppId   - The GPP Id to call the Isr for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvDxExMxGppIsrCall
(
    IN  GT_U8       devNum,
    IN  CPSS_EVENT_GPP_ID_ENT   gppId
);

/*******************************************************************************
* prvCpssDxChGopIsrRegRead
*
* DESCRIPTION:
*       Read ports GOP0-3 MIBs Interrupt Cause register value
*
* INPUTS:
*       devNum  - The PP to read from.
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChGopIsrRegRead
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
);

/*******************************************************************************
* prvCpssDxChPortGroupGopIsrRegRead
*
* DESCRIPTION:
*       Read ports GOP0-3 MIBs Interrupt Cause register value
*       for the specific port group
*
* INPUTS:
*       devNum  - The PP to read from.
*       portGroupId  - port group Id.
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGroupGopIsrRegRead
(
    IN GT_U8 devNum,
    IN GT_U32  portGroupId,
    IN GT_U32 regAddr,
    IN GT_U32 *dataPtr
);

/*******************************************************************************
* prvCpssDrvDxExMxInitObject
*
* DESCRIPTION:
*      This function creates and initializes ExMxDx device driver object
*
* INPUTS:
*       devNum          - The PP device number to read from.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_ALREADY_EXIST - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvDxExMxInitObject
(
     IN GT_U8   devNum
);

/*******************************************************************************
* prvCpssDrvDxExMxEventsInitObject
*
* DESCRIPTION:
*      This function creates and initializes DxExMx device driver object
*
* INPUTS:
*       none
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvDxExMxEventsInitObject
(
     void
);

/*******************************************************************************
* prvCpssDrvDxExMxIntInitObject
*
* DESCRIPTION:
*      This function creates and initializes DxExMx device driver object
*
* INPUTS:
*       none
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvDxExMxIntInitObject
(
     void
);

/*******************************************************************************
* prvCpssDrvPpIntDefTigerInit
*
* DESCRIPTION:
*       Interrupts initialization for the Tiger devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefTigerInit(void);

/*******************************************************************************
* prvCpssDrvPpIntDefTwistCInit
*
* DESCRIPTION:
*       Interrupts initialization for the Twist-C (and Twist-D) devices.
*
* INPUTS:
*       initForTwistD  -  when GT_TRUE the initialization is for Twist-D
*                       otherwise it is for Twist-C
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefTwistCInit(
    IN  GT_BOOL initForTwistD
);

/*******************************************************************************
* prvCpssDrvPpIntDefTwistDInit
*
* DESCRIPTION:
*       Interrupts initialization for the Twist-D devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefTwistDInit(void);

/*******************************************************************************
* prvCpssDrvPpIntDefSalsaInit
*
* DESCRIPTION:
*       Interrupts initialization for the Salsa devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefSalsaInit(void);

/*******************************************************************************
* prvCpssDrvPpIntDefCheetahInit
*
* DESCRIPTION:
*       Interrupts initialization for the Cheetah devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefCheetahInit(void);

/*******************************************************************************
* prvCpssDrvPpIntDefCheetah2Init
*
* DESCRIPTION:
*       Interrupts initialization for the Cheetah-2 devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefCheetah2Init(void);

/*******************************************************************************
* prvCpssDrvPpIntDefCheetah3Init
*
* DESCRIPTION:
*       Interrupts initialization for the Cheetah-3 devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefCheetah3Init(void);

/*******************************************************************************
* prvCpssDrvPpIntDefDxChXcatInit
*
* DESCRIPTION:
*       Interrupts initialization for the DxChXcat devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefDxChXcatInit(void);

/*******************************************************************************
* prvCpssDrvPpIntDefDxChLionInit
*
* DESCRIPTION:
*       Interrupts initialization for the DxChLion devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefDxChLionInit(void);

/*******************************************************************************
* prvCpssDrvPpIntDefPumaInit
*
* DESCRIPTION:
*       Interrupts initialization for the Puma devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefPumaInit(void);

/*******************************************************************************
* prvCpssDrvPpIntExMxIntCauseToUniEvConvert
*
* DESCRIPTION:
*       Converts Interrupt Cause event to unified event type. -- ExMx devices
*
* INPUTS:
*       devNum      - The device number.
*       portGroupId      - The port group Id.
*       intCauseIndex - The interrupt cause to convert.
*
* OUTPUTS:
*       uniEvPtr    - (pointer to)The unified event type.
*       extDataPtr  - (pointer to)The event extended data.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_NOT_FOUND - the interrupt cause to convert was not found
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpIntExMxIntCauseToUniEvConvert
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  GT_U32                      intCauseIndex,
    OUT GT_U32                      *uniEvPtr,
    OUT GT_U32                      *extDataPtr
);

/*******************************************************************************
* prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert
*
* DESCRIPTION:
*       Converts Interrupt Cause event to unified event type. -- cheetah devices
*       --> per Port Group (to support multi-port-groups device)
*
* INPUTS:
*       devNum      - The device number.
*       portGroupId      - The port group Id.
*       intCauseIndex - The interrupt cause to convert.
*
* OUTPUTS:
*       uniEvPtr    - (pointer to)The unified event type.
*       extDataPtr  - (pointer to)The event extended data.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_NOT_FOUND - the interrupt cause to convert was not found
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  GT_U32                      intCauseIndex,
    OUT GT_U32                      *uniEvPtr,
    OUT GT_U32                      *extDataPtr
);


/*******************************************************************************
* prvCpssDrvPpIntCheetahUniEvToIntCauseConvert
*
* DESCRIPTION:
*       Converts unified event type and extra data to Interrupt Cause event
*       -- cheetah devices
*
* INPUTS:
*       devNum      - The device number.
*       uniEv       - The unified event type to convert.
*       extData     - The event extended data to convert.
*
* OUTPUTS:
*       intCauseIndexPtr - (pointer to)The interrupt cause.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_NOT_FOUND - the uniEv and extData to convert was not found
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpIntCheetahUniEvToIntCauseConvert
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      uniEv,
    IN  GT_U32                      extData,
    OUT  GT_U32                     *intCauseIndexPtr
);

/*******************************************************************************
* prvCpssDrvPpIntSalsaIntCauseToUniEvConvert
*
* DESCRIPTION:
*       Converts Interrupt Cause event to unified event type. -- Salsa devices
*
* INPUTS:
*       devNum      - The device number.
*       portGroupId      - The portGroupId. support the multi-port-groups device.
*                     ignored for non multi-port-groups device.
*       intCauseIndex - The interrupt cause to convert.
*
* OUTPUTS:
*       uniEvPtr    - (pointer to)The unified event type.
*       extDataPtr  - (pointer to)The event extended data.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_NOT_FOUND - the interrupt cause to convert was not found
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpIntSalsaIntCauseToUniEvConvert
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  GT_U32                      intCauseIndex,
    OUT GT_U32                      *uniEvPtr,
    OUT GT_U32                      *extDataPtr
);

/*******************************************************************************
* prvCpssDrvPpIntPumaIntCauseToUniEvConvert
*
* DESCRIPTION:
*       Converts Interrupt Cause event to unified event type. -- Puma devices
*
* INPUTS:
*       devNum      - The device number.
*       portGroupId      - The portGroupId. support the multi-port-groups device.
*                     ignored for non multi-port-groups device.
*       intCauseIndex - The interrupt cause to convert.
*
* OUTPUTS:
*       uniEvPtr    - (pointer to)The unified event type.
*       extDataPtr  - (pointer to)The event extended data.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_NOT_FOUND - the interrupt cause to convert was not found
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpIntPumaIntCauseToUniEvConvert
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  GT_U32                      intCauseIndex,
    OUT GT_U32                      *uniEvPtr,
    OUT GT_U32                      *extDataPtr
);

/*******************************************************************************
* prvCpssDrvPpIntPumaUniEvToIntCauseConvert
*
* DESCRIPTION:
*       Converts unified event type and extra data to Interrupt Cause event
*       -- ExMxPm devices
*
* INPUTS:
*       devNum      - The device number.
*       uniEv       - The unified event type to convert.
*       extData     - The event extended data to convert.
*
* OUTPUTS:
*       intCauseIndexPtr - (pointer to)The interrupt cause.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_NOT_FOUND - the uniEv and extData to convert was not found
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpIntPumaUniEvToIntCauseConvert
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      uniEv,
    IN  GT_U32                      extData,
    OUT  GT_U32                     *intCauseIndexPtr
);


/*******************************************************************************
* prvCpssDrvExMxDxHwPpInterruptsTreeGet
*
* DESCRIPTION:
*       function return :
*       1. the root to the interrupts tree info of the specific device
*       2. the interrupt registers that can't be accesses before 'Start Init'
*
* APPLICABLE DEVICES: All devices --> Packet Processors (not Fa/Xbar)
*
* INPUTS:
*       devNum - the device number
*       portGroupId          - The port group Id. relevant only to multi port group devices
*
* OUTPUTS:
*       numOfElementsPtr - (pointer to) number of elements in the tree.
*       treeRootPtrPtr - (pointer to) pointer to root of the interrupts tree info.
*       numOfInterruptRegistersNotAccessibleBeforeStartInitPtr - (pointer to)
*                           number of interrupt registers that can't be accessed
*                           before 'Start init'
*       notAccessibleBeforeStartInitPtrPtr (pointer to)pointer to the interrupt
*                           registers that can't be accessed before 'Start init'
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - the driver was not initialized for the device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   prvCpssDrvExMxDxHwPpInterruptsTreeGet
(
    IN GT_U8    devNum,
    IN  GT_U32          portGroupId,
    OUT GT_U32  *numOfElementsPtr,
    OUT const CPSS_INTERRUPT_SCAN_STC        **treeRootPtrPtr,
    OUT GT_U32  *numOfInterruptRegistersNotAccessibleBeforeStartInitPtr,
    OUT GT_U32  **notAccessibleBeforeStartInitPtrPtr
);


/*******************************************************************************
* prvCpssDrvExMxDxHwPpMaskRegInfoGet
*
* DESCRIPTION:
*      function to build from the interrupts tree and the mask registers
*      addresses , the content of the summary bits in those mask registers.
*      1. fill the array of mask registers addresses
*      2. fill the array of default values for the mask registers
*      3. update the bits of nonSumBitMask in the scan tree
*
* INPUTS:
*       devFamily - device family
*       numScanElements - number of elements in intrScanArr[]
*       intrScanArr[] - interrupts scan info array
*       numMaskRegisters - number of registers in maskRegMapArr,maskRegDefaultSummaryArr
*       maskRegMapArr[]            - (pointer to) place holder for the mask registers addresses
*       maskRegDefaultSummaryArr[] - (pointer to) place holder for the registers values
*
* OUTPUTS:
*       intrScanArr[ii].nonSumBitMask - non summary bits updated
*       maskRegMapArr[]     - (pointer to) the mask registers addresses
*       maskRegDefaultSummaryArr[] - (pointer to) the registers values
*
* RETURNS:
*       GT_OK   - on success,
*       GT_OUT_OF_CPU_MEM - fail to allocate cpu memory (osMalloc)
*       GT_BAD_PARAM - the scan tree information has error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvExMxDxHwPpMaskRegInfoGet
(
    IN  CPSS_PP_FAMILY_TYPE_ENT devFamily,
    IN  GT_U32  numScanElements,
    INOUT PRV_CPSS_DRV_INTERRUPT_SCAN_STC intrScanArr[],
    IN  GT_U32   numMaskRegisters,
    OUT GT_U32   maskRegDefaultSummaryArr[],
    OUT GT_U32   maskRegMapArr[]
);

    #define DUMP_DEFAULT_INFO

#ifdef _WIN32
    #define DUMP_DEFAULT_INFO
#endif /*_WIN32*/

#ifdef DUMP_DEFAULT_INFO
/*******************************************************************************
* prvCpssDrvPpIntDefPrint
*
* DESCRIPTION:
*      print the interrupts arrays info, that build by
*      prvCpssDrvExMxDxHwPpMaskRegInfoGet
*
* INPUTS:
*       numScanElements - number of elements in intrScanArr[]
*       intrScanArr[] - interrupts scan info array
*       numMaskRegisters - number of registers in maskRegMapArr,maskRegDefaultSummaryArr
*       maskRegMapArr[]            - (pointer to) place holder for the mask registers addresses
*       maskRegDefaultSummaryArr[] - (pointer to) place holder for the registers values
*
* OUTPUTS:
*
* RETURNS:
*       void
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void  prvCpssDrvPpIntDefPrint(
    IN GT_U32  numScanElements,
    IN CPSS_INTERRUPT_SCAN_STC intrScanArr[],
    IN GT_U32  numMaskRegisters,
    IN GT_U32  maskRegMapArr[] ,
    IN GT_U32  maskRegDefaultSummaryArr[]
);
#endif/*DUMP_DEFAULT_INFO*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvDxExMxInterrupts */


