/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChDiag.h
*
* DESCRIPTION:
*       Diag APIs for CPSS DxCh.
*
* PRBS sequence for tri-speed ports:
*  1. Enable PRBS checker on receiver port (cpssDxChDiagPrbsPortCheckEnableSet)
*  2. Check that checker initialization is done (cpssDxChDiagPrbsPortCheckReadyGet)
*  3. Set CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS_E transmit mode on transmiting port
*     (cpssDxChDiagPrbsPortTransmitModeSet)
*  4. Enable PRBS generator on transmiting port (cpssDxChDiagPrbsPortGenerateEnableSet)
*  5. Check results on receiving port (cpssDxChDiagPrbsPortStatusGet)
*
* PRBS sequence for XG ports:
*  1. Set CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E or CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS23_E
*      transmit mode on both ports(cpssDxChDiagPrbsPortTransmitModeSet)
*  2. Enable PRBS checker on receiver port (cpssDxChDiagPrbsPortCheckEnableSet)
*  3. Enable PRBS generator on transmiting port (cpssDxChDiagPrbsPortGenerateEnableSet)
*  4. Check results on receiving port (cpssDxChDiagPrbsPortStatusGet)
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*
*******************************************************************************/
#ifndef __cpssDxChDiagh
#define __cpssDxChDiagh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/generic/diag/cpssDiag.h>
/*
 * Typedef: CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT
 *
 * Description: Transmit modes.
 *
 * Enumerations:
 *      CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E -
 *          Regular Mode: Input is from the MAC PCS Tx block.
 *      CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS_E -
            PRBS Mode: 1.25 Gbps input is from the PRBS Generator.
 *      CPSS_DXCH_DIAG_TRANSMIT_MODE_ZEROS_E - Zeros Constant.
 *      CPSS_DXCH_DIAG_TRANSMIT_MODE_ONES_E  - Ones Constant.
 *      CPSS_DXCH_DIAG_TRANSMIT_MODE_CYCLIC_E -
 *         Cyclic Data; The data in Cyclic Date Register 0 - 3 is transmitted.
 *      CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E - PRBS7.
 *      CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS23_E  - PRBS23.
 */
typedef enum
{
    CPSS_DXCH_DIAG_TRANSMIT_MODE_REGULAR_E,
    CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS_E,     /* for gig ports only */
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ZEROS_E,    /* for gig ports only */
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ONES_E,     /* for gig ports only */
    CPSS_DXCH_DIAG_TRANSMIT_MODE_CYCLIC_E,   /* for XG ports only */
    CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E,    /* for XG ports only */
    CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS23_E    /* for XG ports only */
} CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT  ;

/*
 * Typedef: CPSS_DIAG_PP_MEM_BIST_TYPE_ENT
 *
 * Description: Packet Process memory type for BIST.
 *
 * Enumerations:
 *      CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E     - The PCL TCAM.
 *      CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E  - The ROUTER TCAM.
 */
typedef enum
{
    CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E,
    CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E

}CPSS_DIAG_PP_MEM_BIST_TYPE_ENT;


/*******************************************************************************
* cpssDxChDiagMemTest
*
* DESCRIPTION:
*       Performs memory test on a specified memory location and size for a
*       specified memory type.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number to test
*       memType     - The packet processor memory type to verify.
*       startOffset - The offset address to start the test from.
*       size        - The memory size in byte to test (start from offset).
*       profile     - The test profile.
*
* OUTPUTS:
*       testStatusPtr  - (pointer to) test status. GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - (pointer to) address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - (pointer to) value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - (pointer to) value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong memory type
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The test is done by writing and reading a test pattern.
*       The function may be called after Phase 2 initialization.
*       The test is destructive and leaves the memory corrupted.
*       Supported memories:
*       - Buffer DRAM
*       - MAC table memory
*       - VLAN table memory
*       For buffer DRAM:
*          startOffset must be aligned to 64 Bytes and size must be in 64 bytes
*          resolution.
*       For MAC table:
*          startOffset must be aligned to 16 Bytes and size must be in 16 bytes
*          resolution.
*       For VLAN table:
*          DX CH devices: startOffset must be aligned to 12 Bytes and size must
*                         be in 12 bytes resolution.
*          DX CH2 and above devices: startOffset must be aligned to 16 Bytes
*                 and size must be in 16 bytes resolution.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagMemTest
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         startOffset,
    IN GT_U32                         size,
    IN CPSS_DIAG_TEST_PROFILE_ENT     profile,
    OUT GT_BOOL                       *testStatusPtr,
    OUT GT_U32                        *addrPtr,
    OUT GT_U32                        *readValPtr,
    OUT GT_U32                        *writeValPtr
);

/*******************************************************************************
* cpssDxChDiagAllMemTest
*
* DESCRIPTION:
*       Performs memory test for all the internal and external memories.
*       Tested memories:
*       - Buffer DRAM
*       - MAC table memory
*       - VLAN table memory
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number to test
*
* OUTPUTS:
*       testStatusPtr  - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The test is done by invoking cpssDxChDiagMemTest in loop for all the
*       memory types and for AA-55, random and incremental patterns.
*       The function may be called after Phase 2 initialization.
*       The test is destructive and leaves the memory corrupted.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagAllMemTest
(
    IN GT_U8                     devNum,
    OUT GT_BOOL                  *testStatusPtr,
    OUT GT_U32                   *addrPtr,
    OUT GT_U32                   *readValPtr,
    OUT GT_U32                   *writeValPtr
);

/*******************************************************************************
* cpssDxChDiagMemWrite
*
* DESCRIPTION:
*       performs a single 32 bit data write to one of the PP memory spaces.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device number
*       memType - The packet processor memory type
*       offset  - The offset address to write to
*       data    - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_TIMEOUT - after max time that the PP not finished action
*       GT_BAD_PARAM             - on wrong devNum, memType or offset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagMemWrite
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    IN GT_U32                         data
);

/*******************************************************************************
* cpssDxChDiagMemRead
*
* DESCRIPTION:
*       performs a single 32 bit data read from one of the PP memory spaces.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device number
*       memType - The packet processor memory type
*       offset  - The offset address to read from
*
* OUTPUTS:
*       dataPtr - (pointer to) read data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_BAD_PARAM             - on wrong devNum, memType or offset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagMemRead
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    OUT GT_U32                        *dataPtr
);

/*******************************************************************************
* cpssDxChDiagRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PP PCI configuration or
*       internal registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       ifChannel  - interface channel (PCI/SMI/TWSI)
*       regType    - The register type
*       offset     - the register offset
*       data       - data to write
*       doByteSwap - GT_TRUE:  byte swap will be done on the written data
*                    GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagRegWrite
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
);

/*******************************************************************************
* cpssDxChDiagRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PP PCI configuration or
*       internal registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       ifChannel  - interface channel (PCI/SMI/TWSI)
*       regType    - The register type
*       offset     - the register offset
*       doByteSwap - GT_TRUE:  byte swap will be done on the read data
*                    GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - read data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagRegRead
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN CPSS_DIAG_PP_REG_TYPE_ENT       regType,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
);

/*******************************************************************************
* cpssDxChDiagPhyRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PHY registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr     - The base address to access the device
*       ifChannel    - interface channel (PCI/SMI/TWSI)
*       smiRegOffset - The SMI register offset
*       phyAddr      - phy address to access
*       offset       - PHY register offset
*       data         - data to write
*       doByteSwap   - GT_TRUE:  byte swap will be done on the written data
*                      GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPhyRegWrite
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN GT_U32                          smiRegOffset,
    IN GT_U32                          phyAddr,
    IN GT_U32                          offset,
    IN GT_U32                          data,
    IN GT_BOOL                         doByteSwap
);

/*******************************************************************************
* cpssDxChDiagPhyRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PHY registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       baseAddr     - The base address to access the device
*       ifChannel    - interface channel (PCI/SMI/TWSI)
*       smiRegOffset - The SMI register offset
*       phyAddr      - phy address to access
*       offset       - PHY register offset
*       doByteSwap   - GT_TRUE:  byte swap will be done on the read data
*                      GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - (pointer to) read data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPhyRegRead
(
    IN GT_U32                          baseAddr,
    IN CPSS_PP_INTERFACE_CHANNEL_ENT   ifChannel,
    IN GT_U32                          smiRegOffset,
    IN GT_U32                          phyAddr,
    IN GT_U32                          offset,
    OUT GT_U32                         *dataPtr,
    IN GT_BOOL                         doByteSwap
);

/*******************************************************************************
* cpssDxChDiagRegsNumGet
*
* DESCRIPTION:
*       Gets the number of registers for the PP.
*       Used to allocate memory for cpssDxChDiagRegsDump.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*
* OUTPUTS:
*       regsNumPtr    - (pointre to) number of registers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagRegsNumGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *regsNumPtr
);

/*******************************************************************************
* cpssDxChDiagRegsDump
*
* DESCRIPTION:
*       Dumps the device register addresses and values according to the given
*       starting offset and number of registers to dump.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*       regsNumPtr    - (pointer to) number of registers to dump. This number must not be
*                       bigger than the number of registers that can be dumped
*                       (starting at offset).
*       offset        - The first register address to dump
*
* OUTPUTS:
*       regsNumPtr    - (pointer to) number of registers that were dumped
*       regAddrPtr    - (pointer to) addresses of the dumped registers
*                       The addresses are taken from the register DB.
*       regDataPtr    - (pointer to) data in the dumped registers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       To dump all the registers the user may call cpssDxChDiagRegsNumGet in
*       order to get the number of registers of the PP.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagRegsDump
(
    IN    GT_U8     devNum,
    INOUT GT_U32    *regsNumPtr,
    IN    GT_U32    offset,
    OUT   GT_U32    *regAddrPtr,
    OUT   GT_U32    *regDataPtr
);

/*******************************************************************************
* cpssDxChDiagRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of a specific register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*       regAddr       - Register to test
*       regMask       - Register mask. The test verifies only the non-masked
*                       bits.
*       profile       - The test profile
*
* OUTPUTS:
*       testStatusPtr - (pointer to) to test result. GT_TRUE if the test succeeded or GT_FALSE for failure
*       readValPtr    - (pointer to) value read from the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       writeValPtr   - (pointer to) value written to the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       After the register is tested, the original value prior to the test is
*       restored.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagRegTest
(
    IN GT_U8                          devNum,
    IN GT_U32                         regAddr,
    IN GT_U32                         regMask,
    IN CPSS_DIAG_TEST_PROFILE_ENT     profile,
    OUT GT_BOOL                       *testStatusPtr,
    OUT GT_U32                        *readValPtr,
    OUT GT_U32                        *writeValPtr
);

/*******************************************************************************
* cpssDxChDiagAllRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of all the registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*
* OUTPUTS:
*       testStatusPtr - (pointer to) test result. GT_TRUE if the test succeeded or GT_FALSE for failure
*       badRegPtr     - (pointer to) address of the register which caused the failure if
*                       testStatusPtr is GT_FALSE. Irrelevant if testStatusPtr
*                       is GT_TRUE.
*       readValPtr    - (pointer to) value read from the register which caused the
*                       failure if testStatusPtr is GT_FALSE. Irrelevant if
*                       testStatusPtr is GT_TRUE
*       writeValPtr   - (pointer to) value written to the register which caused the
*                       failure if testStatusPtr is GT_FALSE. Irrelevant if
*                       testStatusPtr is GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The test is done by invoking cpssDxChDiagRegTest in loop for all the
*       diagnostics registers and for all the patterns.
*       After each register is tested, the original value prior to the test is
*       restored.
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagAllRegTest
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *testStatusPtr,
    OUT GT_U32  *badRegPtr,
    OUT GT_U32  *readValPtr,
    OUT GT_U32  *writeValPtr
);

/*******************************************************************************
* cpssDxChDiagPrbsPortTransmitModeSet
*
* DESCRIPTION:
*       Set transmit mode for specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       mode     - transmit mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       CPU port doesn't support the transmit mode.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortTransmitModeSet
(
    IN   GT_U8                    devNum,
    IN   GT_U8                    portNum,
    IN   GT_U32                   laneNum,
    IN   CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT   mode
);

/*******************************************************************************
* cpssDxChDiagPrbsPortTransmitModeGet
*
* DESCRIPTION:
*       Get transmit mode for specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       modePtr  - (pointer to) transmit mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       CPU port doesn't support the transmit mode.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortTransmitModeGet
(
    IN   GT_U8                    devNum,
    IN   GT_U8                    portNum,
    IN   GT_U32                   laneNum,
    OUT  CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT   *modePtr
);

/*******************************************************************************
* cpssDxChDiagPrbsPortGenerateEnableSet
*
* DESCRIPTION:
*       Enable/Disable PRBS (Pseudo Random Bit Generator) pattern generation
*       per Port and per lane.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       enable   - GT_TRUE - PRBS pattern generation is enabled
*                  GT_FALSE - PRBS pattern generation is disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       When operating, enable the PRBS checker before the generator.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortGenerateEnableSet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    IN   GT_BOOL        enable
);

/*******************************************************************************
* cpssDxChDiagPrbsPortGenerateEnableGet
*
* DESCRIPTION:
*       Get the status of PRBS (Pseudo Random Bit Generator) pattern generation
*       per port and per lane.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - PRBS pattern generation is enabled
*                     GT_FALSE - PRBS pattern generation is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortGenerateEnableGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_BOOL        *enablePtr
);

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable PRBS (Pseudo Random Bit Generator) checker per port and
*       per lane.
*       When the checker is enabled, it seeks to lock onto the incoming bit
*       stream, and once this is achieved the PRBS checker starts counting the
*       number of bit errors. Tne number of errors can be retrieved by
*       cpssDxChDiagPrbsGigPortStatusGet API.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       enable   - GT_TRUE - PRBS checker is enabled
*                  GT_FALSE - PRBS checker is disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       When operating, enable the PRBS checker before the generator.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortCheckEnableSet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    IN   GT_BOOL        enable
);

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckEnableGet
*
* DESCRIPTION:
*       Get the status (enabled or disabled) of PRBS (Pseudo Random Bit Generator)
*       checker per port and per lane.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       enablePtr   - (pointer to) PRBS checker state.
*                     GT_TRUE - PRBS checker is enabled
*                     GT_FALSE - PRBS checker is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortCheckEnableGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_BOOL        *enablePtr
);

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckReadyGet
*
* DESCRIPTION:
*       Get the PRBS checker ready status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       isReadyPtr  - (pointer to) PRBS checker state.
*                     GT_TRUE - PRBS checker is ready.
*                     PRBS checker has completed the initialization phase.
*                     GT_FALSE - PRBS checker is not ready.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the Tri-speed ports.
*       The Check ready status indicates that the PRBS checker has completed
*       the initialization phase. The PRBS generator at the transmit side may
*       be enabled.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortCheckReadyGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    OUT  GT_BOOL        *isReadyPtr
);

/*******************************************************************************
* cpssDxChDiagPrbsPortStatusGet
*
* DESCRIPTION:
*       Get PRBS (Pseudo Random Bit Generator) Error Counter and Checker Locked
*       status per port and per lane.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       checkerLockedPtr        - (pointer to) checker locked state.
*                                 GT_TRUE - checker is locked on the sequence stream.
*                                 GT_FALSE - checker isn't locked on the sequence
*                                           stream.
*       errorCntrPtr            - (pointer to) PRBS Error counter. This counter represents
*                                 the number of bit mismatches detected since
*                                 the PRBS checker of the port has locked.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this device type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PRBS Error counter is cleared on read.
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsPortStatusGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_BOOL        *checkerLockedPtr,
    OUT  GT_U32         *errorCntrPtr
);

/*******************************************************************************
* cpssDxChDiagPrbsCyclicDataSet
*
* DESCRIPTION:
*       Set cylic data for transmition. See cpssDxChDiagPrbsPortTransmitModeSet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*       laneNum         - lane number
*       cyclicDataArr[4] - cyclic data array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_SUPPORTED         - request is not supported for this port type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the XAUI/HGS ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsCyclicDataSet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    IN   GT_U32         cyclicDataArr[4]
);

/*******************************************************************************
* cpssDxChDiagPrbsCyclicDataGet
*
* DESCRIPTION:
*       Get cylic data for transmition. See cpssDxChDiagPrbsPortTransmitModeSet.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*       laneNum         - lane number
*
* OUTPUTS:
*       cyclicDataArr[4]  - cyclic data array
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this port type.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the XAUI/HGS ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsCyclicDataGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_U32         cyclicDataArr[4]
);

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTestEnableSet
*
* DESCRIPTION:
*       Enable/Disable SERDES PRBS (Pseudo Random Bit Generator) test mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*       enable    - GT_TRUE - test enabled
*                   GT_FALSE - test disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.Transmit mode should be set before enabling test mode.
*       See test cpssDxChDiagPrbsSerdesTransmitModeSet.
*       2. Supported only for GE and FlexLink ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsSerdesTestEnableSet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    IN   GT_BOOL        enable
);

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTestEnableGet
*
* DESCRIPTION:
*       Get the status of PRBS (Pseudo Random Bit Generator) test mode.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - test enabled
*                     GT_FALSE - test disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported only for GE and FlexLink ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsSerdesTestEnableGet
(
    IN   GT_U8          devNum,
    IN   GT_U8          portNum,
    IN   GT_U32         laneNum,
    OUT  GT_BOOL       *enablePtr
);

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTransmitModeSet
*
* DESCRIPTION:
*       Set transmit mode for SERDES PRBS on specified port/lane.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*       mode      - transmit mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*                                  or unsupported transmit mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Only CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E supported.
*       2. Supported only for GE and FlexLink ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsSerdesTransmitModeSet
(
    IN   GT_U8                            devNum,
    IN   GT_U8                            portNum,
    IN   GT_U32                           laneNum,
    IN   CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode
);

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTransmitModeGet
*
* DESCRIPTION:
*       Get transmit mode for SERDES PRBS on specified port/lane.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
* OUTPUTS:
*       modePtr   - transmit mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on unkonown transmit mode
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported only for GE and FlexLink ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsSerdesTransmitModeGet
(
    IN   GT_U8                             devNum,
    IN   GT_U8                             portNum,
    IN   GT_U32                            laneNum,
    OUT  CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT *modePtr
);

/*******************************************************************************
* cpssDxChDiagPrbsSerdesStatusGet
*
* DESCRIPTION:
*       Get SERDES PRBS (Pseudo Random Bit Generator) pattern detector state,
*       error counter and pattern counter.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*
* OUTPUTS:
*       lockedPtr      - (pointer to) Pattern detector state.
*                         GT_TRUE - Pattern detector had locked onto the pattern.
*                         GT_FALSE - Pattern detector is not locked onto
*                                   the pattern.

*       errorCntrPtr   - (pointer to) PRBS Error counter. This counter represents
*                                 the number of bit mismatches detected since
*                                 the PRBS checker of the port has locked.
*       patternCntrPtr - (pointer to) Pattern counter. Number of 40-bit patterns
*                                 received since acquiring pattern lock.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported only for GE and FlexLink ports.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPrbsSerdesStatusGet
(
    IN   GT_U8     devNum,
    IN   GT_U8     portNum,
    IN   GT_U32    laneNum,
    OUT  GT_BOOL  *lockedPtr,
    OUT  GT_U32   *errorCntrPtr,
    OUT  GT_U64   *patternCntrPtr
);

/*******************************************************************************
* cpssDxChDiagMemoryBistsRun
*
* DESCRIPTION:
*       Runs BIST (Built-in self-test) on specified memory.
*
* APPLICABLE DEVICES:
*        DxCh3; Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat2.
*
* INPUTS:
*       devNum        - device number
*       memBistType   - type of memory
*       timeOut       - maximal time in milisecond to wait for BIST finish.
*       clearMemoryAfterTest - GT_TRUE  - Clear memory after test.
*                              GT_FALSE - Don't clear memory after test.
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT    - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_UNFIXABLE_BIST_ERROR  - on unfixable problem in the TCAM
*
* COMMENTS:
*       1. This function should not be called under traffic.
*       2. Bist destroys the content of the memory. If clearMemoryAfterTest set
*          function will clear the content of the memory after the test.
*          Application responsible to restore the content after the function
*          completion.
*******************************************************************************/
GT_STATUS cpssDxChDiagMemoryBistsRun
(
    IN  GT_U8                                devNum,
    IN  CPSS_DIAG_PP_MEM_BIST_TYPE_ENT       memBistType,
    IN  GT_U32                               timeOut,
    IN  GT_BOOL                              clearMemoryAfterTest
);

/*******************************************************************************
* cpssDxChDiagMemoryBistBlockStatusGet
*
* DESCRIPTION:
*       Gets redundancy block status.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; Lion; xCat2.
*
* INPUTS:
*       devNum        - device number
*       memBistType   - type of memory
*       blockIndex    - block index
*                       CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E:
*                         valid blockIndex: 0 to 13.
*                       CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E:
*                         valid blockIndex: 0 to 19.
*
* OUTPUTS:
*       blockFixedPtr   - (pointer to) block status
*                                 GT_TRUE - row in the block was fixed
*                                 GT_FALSE - row in the block wasn't fixed
*       replacedIndexPtr - (pointer to) replaced row index.
*                                Only valid if  *blockFixedPtr is GT_TRUE.
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*          Each redundancy block contains 256 rows. Block 0: rows 0 to 255,
*          block 1: raws 256 to 511 ...
*          One raw replacement is possible though the full redundancy block
*          of 256 rows.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagMemoryBistBlockStatusGet
(
    IN  GT_U8                                devNum,
    IN  CPSS_DIAG_PP_MEM_BIST_TYPE_ENT       memBistType,
    IN  GT_U32                               blockIndex,
    OUT GT_BOOL                              *blockFixedPtr,
    OUT GT_U32                               *replacedIndexPtr
);

/*******************************************************************************
* cpssDxChDiagDeviceTemperatureGet
*
* DESCRIPTION:
*       Gets the PP temperature.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat.
*
* INPUTS:
*       devNum          - device number
* OUTPUTS:
*       temperaturePtr  - (pointer to) temperature in Celsius degrees
*                         (can be negative)
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChDiagDeviceTemperatureGet
(
    IN  GT_U8    devNum,
    OUT GT_32    *temperaturePtr
);

/*******************************************************************************
* cpssDxChDiagPortGroupMemWrite
*
* DESCRIPTION:
*       performs a single 32 bit data write to one of the specific port group in
*       PP memory spaces.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device number
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       memType     - The packet processor memory type
*       offset      - The offset address to write to
*       data        - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_TIMEOUT - after max time that the PP not finished action
*       GT_BAD_PARAM             - on wrong devNum, memType or offset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPortGroupMemWrite
(
    IN GT_U8                          devNum,
    IN GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    IN GT_U32                         data
);


/*******************************************************************************
* cpssDxChDiagPortGroupMemRead
*
* DESCRIPTION:
*       performs a single 32 bit data read from one of the specific port group
*       in the PP memory spaces.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported,
*                          in this case read is done from first active port group.
*       memType     - The packet processor memory type
*       offset      - The offset address to read from
*
* OUTPUTS:
*       dataPtr - (pointer to) read data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_BAD_PARAM             - on wrong devNum, memType or offset
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPortGroupMemRead
(
    IN GT_U8                          devNum,
    IN GT_PORT_GROUPS_BMP             portGroupsBmp,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    OUT GT_U32                        *dataPtr
);

/*******************************************************************************
* cpssDxChDiagPortGroupRegsNumGet
*
* DESCRIPTION:
*       Gets the number of registers for the specific port group in the PP.
*       Used to allocate memory for cpssDxChDiagRegsDump.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - The device number
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported,
*                          in this case read is done from first active port group.
*
* OUTPUTS:
*       regsNumPtr    - (pointre to) number of registers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPortGroupRegsNumGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_U32              *regsNumPtr
);

/*******************************************************************************
* cpssDxChDiagPortGroupRegsDump
*
* DESCRIPTION:
*       Dumps the device register addresses and values for the specific port
*       group in the PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - The device number
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported,
*                          in this case read is done from first active port group.
*       regsNumPtr    - (pointer to) number of registers to dump. This number must not be
*                       bigger than the number of registers that can be dumped
*                       (starting at offset).
*       offset        - The first register address to dump
*
* OUTPUTS:
*       regsNumPtr    - (pointer to) number of registers that were dumped
*       regAddrPtr    - (pointer to) addresses of the dumped registers
*       regDataPtr    - (pointer to) data in the dumped registers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       To dump all the registers the user may call cpssDxChDiagRegsNumGet in
*       order to get the number of registers of the PP.
*
*******************************************************************************/
GT_STATUS cpssDxChDiagPortGroupRegsDump
(
    IN    GT_U8                 devNum,
    IN    GT_PORT_GROUPS_BMP    portGroupsBmp,
    INOUT GT_U32                *regsNumPtr,
    IN    GT_U32                offset,
    OUT   GT_U32                *regAddrPtr,
    OUT   GT_U32                *regDataPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChDiagh */

