/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChHwInitLedCtrl.h
*
* DESCRIPTION:
*       Includes Leds control functions declarations.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#ifndef __cpssDxChHwInitLedCtrlh
#define __cpssDxChHwInitLedCtrlh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssHwInit/cpssLedCtrl.h>

#define CPSS_DXCH_MAX_LED_INTERFACE_CNS   2   /* max number of led interfaces         */
#define CPSS_DXCH_MAX_LED_CLASS_CNS       7   /* max number of led indication classes */
#define CPSS_DXCH_MAX_LED_GROUP_CNS       4   /* max number of led groups             */

/*
 * typedef: struct CPSS_DXCH_LED_CLASS_CONF_STC
 *
 * Description:
 *      led class indication configuration.
 *
 * Fields:
 *      classNum    - the class number.
 *      classConf   - the class manipulation register value for classNum.
 */
typedef struct CPSS_DXCH_LED_CLASS_CONF_STCT
{
    GT_U32      classNum;
    GT_U32      classConf;
} CPSS_DXCH_LED_CLASS_CONF_STC;


/*
 * typedef: struct CPSS_DXCH_LED_GROUP_CONF_STC
 *
 * Description:
 *      led group configuration.
 *
 * Fields:
 *      groupNum    - the group number.
 *      groupConf   - the group logic register value for groupNum.
 */
typedef struct CPSS_DXCH_LED_GROUP_CONF_STC
{
    GT_U32      groupNum;
    GT_U32      groupConf;
} CPSS_DXCH_LED_GROUP_CONF_STC;

/*
 * typedef: enum CPSS_DXCH_LED_CPU_OR_PORT27_ENT
 *
 * Description: CPU port or Port 27 indication display on
 *              LED interface 0.
 *
 * Enumerations:
 *      CPSS_DXCH_LED_CPU_E     - select cpu port indication.
 *      CPSS_DXCH_LED_PORT27_E  - select port27 indication.
 */
typedef enum
{
    CPSS_DXCH_LED_CPU_E,
    CPSS_DXCH_LED_PORT27_E
} CPSS_DXCH_LED_CPU_OR_PORT27_ENT;

/*
 * typedef: enum CPSS_DXCH_LED_PORT_TYPE_ENT
 *
 * Description: Port type to which the class manipulation
 *              or group configuration is relevant.
 *
 * Enumerations:
 *      CPSS_DXCH_LED_PORT_TYPE_TRI_SPEED_E   - Tri-speed port.
 *                                              For DxChXcat: Network port.
 *                                              For Lion: GE or XG port.
 *      CPSS_DXCH_LED_PORT_TYPE_XG_E          - XG port.
 *                                              For DxChXcat: Flex port.
 *                                              For Lion: Reserved.
 */
typedef enum
{
    CPSS_DXCH_LED_PORT_TYPE_TRI_SPEED_E, 
    CPSS_DXCH_LED_PORT_TYPE_XG_E
} CPSS_DXCH_LED_PORT_TYPE_ENT;

/*
 * typedef: enum CPSS_DXCH_LED_INDICATION_ENT
 *
 * Description: Ports (debug) data indications.
 *
 * Enumerations:
 *      CPSS_DXCH_LED_INDICATION_PRIMARY_E                     - primary indication.
 *      CPSS_DXCH_LED_INDICATION_LINK_E                        - link.
 *      CPSS_DXCH_LED_INDICATION_RX_ACT_E                      - receive activity.
 *      CPSS_DXCH_LED_INDICATION_TX_ACT_E                      - transmit activity.
 *      CPSS_DXCH_LED_INDICATION_LOCAL_FAULT_OR_PCS_LINK_E     - XG mode: local fault
 *                                                               SGMII mode: PCS link
 *      CPSS_DXCH_LED_INDICATION_REMOTE_FAULT_OR_RESERVED_E    - XG mode: remote fault
 *                                                               SGMII mode: reserved (set to 0x0)
 *      CPSS_DXCH_LED_INDICATION_UKN_SEQ_OR_DUPLEX_E           - XG mode: unknown sequence
 *                                                               SGMII mode: duplex
 *      CPSS_DXCH_LED_INDICATION_P_REJ_E                       - p rej.
 *      CPSS_DXCH_LED_INDICATION_RX_ERROR_E                    - receive error.
 *      CPSS_DXCH_LED_INDICATION_JABBER_E                      - jabber.
 *      CPSS_DXCH_LED_INDICATION_FRAGMENT_E                    - fragment.
 *      CPSS_DXCH_LED_INDICATION_CRC_ERROR_E                   - CRC error.
 *      CPSS_DXCH_LED_INDICATION_FC_RX_E                       - flow control received.
 *      CPSS_DXCH_LED_INDICATION_TX_BAD_CRC_OR_LATE_COL_E      - XG mode: transmit bad CRC
 *                                                               SGMII mode: late collision
 *      CPSS_DXCH_LED_INDICATION_RX_BUFFER_FULL_OR_BACK_PR_E   - XG mode: receive buffer full
 *                                                               SGMII mode: back pressure
 *      CPSS_DXCH_LED_INDICATION_WATCHDOG_EXP_SAMPLE_E         - watchdog expired sample.
 */
typedef enum
{
    CPSS_DXCH_LED_INDICATION_PRIMARY_E,
    CPSS_DXCH_LED_INDICATION_LINK_E,
    CPSS_DXCH_LED_INDICATION_RX_ACT_E,
    CPSS_DXCH_LED_INDICATION_TX_ACT_E,
    CPSS_DXCH_LED_INDICATION_LOCAL_FAULT_OR_PCS_LINK_E,
    CPSS_DXCH_LED_INDICATION_REMOTE_FAULT_OR_RESERVED_E,
    CPSS_DXCH_LED_INDICATION_UKN_SEQ_OR_DUPLEX_E,
    CPSS_DXCH_LED_INDICATION_P_REJ_E,
    CPSS_DXCH_LED_INDICATION_RX_ERROR_E,
    CPSS_DXCH_LED_INDICATION_JABBER_E,
    CPSS_DXCH_LED_INDICATION_FRAGMENT_E,
    CPSS_DXCH_LED_INDICATION_CRC_ERROR_E,
    CPSS_DXCH_LED_INDICATION_FC_RX_E,
    CPSS_DXCH_LED_INDICATION_TX_BAD_CRC_OR_LATE_COL_E,
    CPSS_DXCH_LED_INDICATION_RX_BUFFER_FULL_OR_BACK_PR_E,
    CPSS_DXCH_LED_INDICATION_WATCHDOG_EXP_SAMPLE_E
} CPSS_DXCH_LED_INDICATION_ENT;

/*******************************************************************************
* cpssDxChLedStreamClassAndGroupConfig
*
* DESCRIPTION:
*       This routine configures the control, the class manipulation and the
*       group configuration registers for both led interfaces in Dx.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum - physical device number
*       ledControl      - The user interface control register value.
*       ledClassConfPtr- (pointer to) a list of class manipulation registers
*       ledClassNum     - the class list length
*       ledGroupConfPtr- (pointer to) a list of group configuration registers
*       ledGroupNum     - the group list length
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE - ledClassNum value or ledGroupNum value is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  See the Led Serial Interface in the device's data-sheet for full
*           detail of the leds configuration, classes and groups.
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamClassAndGroupConfig
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              ledControl,
    IN  CPSS_DXCH_LED_CLASS_CONF_STC        *ledClassConfPtr,
    IN  GT_U32                              ledClassNum,
    IN  CPSS_DXCH_LED_GROUP_CONF_STC        *ledGroupConfPtr,
    IN  GT_U32                              ledGroupNum
);


/*******************************************************************************
* cpssDxChLedStreamHyperGStackTxQStatusEnableSet
*
* DESCRIPTION:
*       Enables the display to HyperGStack Ports Transmit Queue Status via LED
*       Stream.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum   - physical device number
*       enable  - GT_TRUE - enable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           conveyed via the LED stream. Port<i>TxqNotFull indication for each of
*                           those ports is set to 0 when the number of buffers currently allocated in all
*                           of this ports transmit queues exceeds the limit configured for this port.
*                 GT_FALSE - disable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           not conveyed via the LED stream.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChLedStreamHyperGStackTxQStatusEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_BOOL     enable
);


/*******************************************************************************
* cpssDxChLedStreamHyperGStackTxQStatusEnableGet
*
* DESCRIPTION:
*       Gets HyperGStack Ports Transmit Queue Status via LED Stream.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE - enable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           conveyed via the LED stream. Port<i>TxqNotFull indication for each of
*                           those ports is set to 0 when the number of buffers currently allocated in all
*                           of this ports transmit queues exceeds the limit configured for this port.
*                   GT_FALSE - disable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           not conveyed via the LED stream.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChLedStreamHyperGStackTxQStatusEnableGet
(
    IN  GT_U8       devNum,
    OUT  GT_BOOL    *enablePtr
);

/*******************************************************************************
* cpssDxChLedStreamPortGroupConfigSet
*
* DESCRIPTION:
*       This routine configures the LED stream.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANG           - ledStart or ledEnd out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupConfigSet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32              ledInterfaceNum,
    IN  CPSS_LED_CONF_STC   *ledConfPtr
);

/*******************************************************************************
* cpssDxChLedStreamPortGroupConfigGet
*
* DESCRIPTION:
*       This routine gets the LED stream configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*
* OUTPUTS:
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad hardware value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupConfigGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32              ledInterfaceNum,
    OUT CPSS_LED_CONF_STC   *ledConfPtr
);

/*******************************************************************************
* cpssDxChLedStreamCpuOrPort27ModeSet
*
* DESCRIPTION:
*       This routine configures the whether LED stream interface 0 contains CPU 
*       or port 27 indications.
*
* APPLICABLE DEVICES:  DxCh3 and above exclude Lion
*
* INPUTS:
*       devNum          - physical device number
*       indicatedPort   - the selected port indication: CPU or port 27.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamCpuOrPort27ModeSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_LED_CPU_OR_PORT27_ENT indicatedPort
);

/*******************************************************************************
* cpssDxChLedStreamCpuOrPort27ModeGet
*
* DESCRIPTION:
*       This routine gets the whether LED stream interface 0 contains CPU 
*       or port 27 indications.
*
* APPLICABLE DEVICES:  DxCh3 and above exclude Lion
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       indicatedPortPtr    - (pointer to) the selected port indication: 
*                             CPU or port 27.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamCpuOrPort27ModeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_LED_CPU_OR_PORT27_ENT *indicatedPortPtr
);

/*******************************************************************************
* cpssDxChLedStreamPortGroupClassManipulationSet
*
* DESCRIPTION:
*       This routine configures a LED stream class manipulation.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupClassManipulationSet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  GT_U32                          ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT     portType,
    IN  GT_U32                          classNum,
    IN  CPSS_LED_CLASS_MANIPULATION_STC *classParamsPtr
);

/*******************************************************************************
* cpssDxChLedStreamPortGroupClassManipulationGet
*
* DESCRIPTION:
*       This routine gets a LED stream class manipulation configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*
* OUTPUTS:
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupClassManipulationGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_GROUPS_BMP              portGroupsBmp,
    IN  GT_U32                          ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT     portType,
    IN  GT_U32                          classNum,
    OUT CPSS_LED_CLASS_MANIPULATION_STC *classParamsPtr
);

/*******************************************************************************
* cpssDxChLedStreamPortGroupGroupConfigSet
*
* DESCRIPTION:
*       This routine configures a LED stream group.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       groupNum        - group number 
*                         For tri-speed ports: range 0..3
*                         For XG ports: range 0..1
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - on out of range class number 
*                                  (allowable class range o..15)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupGroupConfigSet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT portType,
    IN  GT_U32                      groupNum,
    IN  CPSS_LED_GROUP_CONF_STC     *groupParamsPtr
);

/*******************************************************************************
* cpssDxChLedStreamPortGroupGroupConfigGet
*
* DESCRIPTION:
*       This routine gets a LED stream group configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       groupNum        - group number 
*                         For tri-speed ports: range 0..3
*                         For XG ports: range 0..1
*
* OUTPUTS:
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamPortGroupGroupConfigGet
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  GT_U32                      ledInterfaceNum,
    IN  CPSS_DXCH_LED_PORT_TYPE_ENT portType,
    IN  GT_U32                      groupNum,
    OUT CPSS_LED_GROUP_CONF_STC     *groupParamsPtr
);

/*******************************************************************************
* cpssDxChLedStreamClassIndicationSet
*
* DESCRIPTION:
*       This routine configures classes 9-11 indication (debug) for flex ports.
*
* APPLICABLE DEVICES:  DxChXcat and above exclude Lion
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*       classNum        - class number, range 9..11
*       indication      - port (debug) indication data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamClassIndicationSet
(
    IN  GT_U8                           devNum,
    IN  GT_U32                          ledInterfaceNum,
    IN  GT_U32                          classNum,
    IN  CPSS_DXCH_LED_INDICATION_ENT    indication
);

/*******************************************************************************
* cpssDxChLedStreamClassIndicationGet
*
* DESCRIPTION:
*       This routine gets classes 9-11 indication (debug) for flex ports.
*
* APPLICABLE DEVICES:  DxChXcat and above exclude Lion
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*       classNum        - class number, range 9..11
*
* OUTPUTS:
*       indicationPtr   - (pointer to) port (debug) indication data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_STATE             - on bad hardware value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamClassIndicationGet
(
    IN  GT_U8                          devNum,
    IN  GT_U32                         ledInterfaceNum,
    IN  GT_U32                         classNum,
    OUT CPSS_DXCH_LED_INDICATION_ENT   *indicationPtr
);

/*******************************************************************************
* cpssDxChLedStreamDirectModeEnableSet
*
* DESCRIPTION:
*       This routine enables\disables LED Direct working mode.
*
* APPLICABLE DEVICES:  DxChXcat and above exclude Lion
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*       enable          - GT_TRUE: direct mode
*                         GT_FALSE: serial mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS: 
*           In Direct mode LED pin indicates a different Stack port status.
*           The external pins are mapped as follows:
*           LEDDATA1 — indicates stack port 24
*           LEDSTB1  — indicates stack port 25
*           LEDDATA0 — indicates stack port 26
*           LEDSTB0  — indicates stack port 27
*           The indication for each port is driven from the LED group0 
*           configurations.
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamDirectModeEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  ledInterfaceNum,
    IN  GT_BOOL enable 
);

/*******************************************************************************
* cpssDxChLedStreamDirectModeEnableGet
*
* DESCRIPTION:
*       This routine gets LED pin indication direct mode enabling status.
*
* APPLICABLE DEVICES:  DxChXcat and above exclude Lion
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE: direct mode
*                                      GT_FALSE: serial mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_STATE             - on bad hardware value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS: 
*           In Direct mode LED pin indicates a different Stack port status.
*           The external pins are mapped as follows:
*           LEDDATA1 — indicates stack port 24
*           LEDSTB1  — indicates stack port 25
*           LEDDATA0 — indicates stack port 26
*           LEDSTB0  — indicates stack port 27
*           The indication for each port is driven from the LED group0 
*           configurations.
*
*******************************************************************************/
GT_STATUS cpssDxChLedStreamDirectModeEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  ledInterfaceNum,
    OUT GT_BOOL *enablePtr  
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChHwInitLedCtrlh */

