/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAppDemoEthPrtCtrl.h
*
* DESCRIPTION: Ethernet Port Control APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#ifndef __gtAppDemoEthPrtCtrlh
#define __gtAppDemoEthPrtCtrlh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <appDemo/os/appOs.h>
#include <appDemo/eventEmulate/gtAppDemoEmulationMsgQueue.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>

#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#endif /*CHX_FAMILY*/

#ifdef EXMXPM_FAMILY
    #include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>
#endif /*EXMXPM_FAMILY*/


/* Number of interrupt queues */
#define     NUM_OF_INT_QUEUES           8

/* Maximum number of segments in a single gtBuf */
#define MAX_GT_BUF_SEG_NUM  100

/************* Pool Buffer Interfaces *****************************************/

/*
 * Typedef: enum GT_ETH_PORT_POOL_TYPE_ENT
 *
 * Description:
 *  Define the buffer pool type.
 *
 */
typedef enum
{
    GT_ETH_PORT_RX_SEG_POOL_E   = 0,
    GT_ETH_PORT_TX_POOL_E       = 1,
    GT_ETH_PORT_MAX_POOLS_E     = 2
} GT_ETH_PORT_POOL_TYPE_ENT;

#include <cmdShell/common/cmdCommon.h>


#define GT_ETH_PORT_NUM_POOLS_CNS     GT_ETH_PORT_MAX_POOLS_E

/*
 * Typedef: CPSS_RX_BUFF_MALLOC_FUNC
 *
 * Description: Prototype of the function pointer to be provided by the
 *              user to perform Rx buffer allocation.
 *              The CPSS invokes this user function when allocating
 *              Rx buffers. This is relevant only if  the DYNAMIC_ALLOC
 *              allocation method is used.
 *
 * Fields:
 *      size   - the size of the Rx buffer to allocate.  This value reflects
 *               the Rx buffer size that the user defines in the
 *               TAPI gtSysConfig.h structure GT_SYS_CONFIG
 *      align - The required byte-alignment for the Rx buffer
 *
 */
typedef GT_U8* (*APPDEMO_CPSS_RX_BUFF_MALLOC_FUNC)
(
    IN GT_U32 size,
    IN GT_U32 align
);

/*******************************************************************************
* appDemoEthPortCreatePool
*
* DESCRIPTION:
*       Create a pool of buffers with pre-allocated memory or allocated memory.
*       Align the pool and buffers.
*
* INPUTS:
*       poolIdx         - the pool index
*       reqBufferSize   - Buffer's requested size
*       numOfBuffers_PTR- number of requested buffers in the pool
*       pool_PTR        - pointer to pool memory allocted by user
*       poolSize        - size of the pointed pool
*       mallocFunc      - Function for allocating the buffers
*
* OUTPUTS:
*       numOfBuffers_PTR- number of buffers actually initialized in the pool
*
* RETURNS:
*       GT_OK           - For successful operation.
*       GT_BAD_VALUE    - incorrect pool index.
*       GT_NO_RESOURCE  - No memory is available to create the pool.
*       GT_NOT_INITIALIZED - In case of un-initialized package.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS appDemoEthPortCreatePool
(
    IN      GT_ETH_PORT_POOL_TYPE_ENT   poolIdx,
    IN      GT_U32                      reqBufferSize,
    INOUT   GT_U32                    * numOfBuffers_PTR,
    IN      GT_U8                     * pool_PTR,
    IN      GT_U32                      poolSize,
    IN      APPDEMO_CPSS_RX_BUFF_MALLOC_FUNC mallocFunc
);


/*******************************************************************************
* appDemoEthPortPoolGetBuf
*
* DESCRIPTION:
*       Return pointer to a buffer from the requested pool
*
* INPUTS:
*       poolIdx - The pool index.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_VOID* - Pointer to the new allocated buffer. NULL is returned in case
*                  no buffer is not available.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID * appDemoEthPortPoolGetBuf
(
    IN  GT_ETH_PORT_POOL_TYPE_ENT poolIdx
);


/*******************************************************************************
* appDemoEthPortPoolFreeBuf
*
* DESCRIPTION:
*       Free a buffer back to its pool.
*
* INPUTS:
*       poolIdx - The pool index.
*       pBuf    - Pointer to buffer to be freed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - For successful operation.
*       GT_BAD_PTR          - The returned buffer does not belong to this pool
*       GT_NOT_INITIALIZED  - In case of un-initialized package.
*       GT_BAD_VALUE        - incorrect pool index.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS appDemoEthPortPoolFreeBuf
(
    IN  GT_ETH_PORT_POOL_TYPE_ENT   poolIdx,
    IN  GT_VOID                   * buf_PTR
);


/*******************************************************************************
* appDemoEthPortPoolGetBufSize
*
* DESCRIPTION:
*       Get the buffer size.
*
* INPUTS:
*       poolIdx - The pool index.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_U32  - the buffer size.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 appDemoEthPortPoolGetBufSize
(
    IN  GT_ETH_PORT_POOL_TYPE_ENT   poolIdx
);

/************* End of Pool Buffer Interfaces **********************************/


/*******************************************************************************
* GT_PACKET_FREE_FUNCPTR
*
* DESCRIPTION:
*       The prototype of the routine to be called to free the received Rx buffer.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       numOfSegments   - The number of segment in segment list.
*       queueNum        - Receive queue number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*GT_PACKET_FREE_FUNCPTR)
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments,
    IN GT_U32           queueNum
);


/*
 * typedef: struct APP_DEMO_ETH_PORT_TX_CTRL
 *
 * Description:  Defines the appDemo ethernet port TX data
 *
 * Fields:
 *      txSem             - semaphore id used for TX.
 *      queueId           - the queue id
 *
 * Comment:
 */
typedef struct
{
    GT_SEM          txSem;
    GT_QUE_ID       queueId;
} APP_DEMO_ETH_PORT_TX_CTRL;


/*
 * typedef: struct APP_DEMO_SEG_STCT
 *
 * Description: Segment list is used by the Rx ISR and the interrupt
 *              handler routine. The first segment pointer is passed
 *              by Queue from ISR to interrupt handler.
 *
 * Values:
 *      len     - the segment length
 *      seg     - pointer to a segment
 *      nextSeg - next segment pointer
 *
 * Comments:
 */
typedef struct APP_DEMO_SEG_STCT
{
    GT_U32               len;
    GT_U8                *seg_PTR;
    struct APP_DEMO_SEG_STCT *nextSeg_PTR;
} APP_DEMO_SEG_STC;


/*
 * typedef: struct APP_DEMO_ETH_PORT_RX_Q_CTRL
 *
 * Description:  Defines the ethernet port RX Queue data
 *
 * Fields:
 *      packetFreeFunc_PTR      - free segments for RXed packet routine.
 *      headerOffset            - the header size added to each packet recieved.
 *      intQueId                - the interrupt queue id.
 *      auQueId                 - the mirrored Address Update queue id used in
 *                                polling mode and controlled learning through
 *                                DSA tag.
 *      numOfRxBufs             - the number of buffers used for Rx.
 *
 * Comment:
 */
typedef struct
{
    GT_PACKET_FREE_FUNCPTR  packetFreeFunc_PTR;
    GT_U32                  headerOffset;
    GT_QUE_ID               intQueId[NUM_OF_INT_QUEUES];
    GT_QUE_ID               auQueId;
    GT_U32                  numOfRxBufs;
} APP_DEMO_ETH_PORT_RX_Q_CTRL;


/*
 * typedef: struct APP_DEMO_ETH_PORT_CTRL_VALID
 *
 * Description:  flag indicating that the control struct is valid
 *
 * Fields:
 *      RxValid   - Rx control struct has been initialized and valid
 *      TxValid   - Tx control struct has been initialized and valid.
 *
 * Comment:
 */
typedef struct
{
    GT_BOOL         RxValid;
    GT_BOOL         TxValid;
} APP_DEMO_ETH_PORT_CTRL_VALID;


/*
 * typedef: struct APP_DEMO_RX_Q_MSG
 *
 * Description: message sent from rx SR to interrupt handler.
 *
 *
 * Values:
 *      rxQueue         - the Rx queue number
 *      seg             - first segment
 *
 * Comments:
 */
typedef struct
{
    GT_U32      rxQueue;
    GT_U32      seg;
} APP_DEMO_RX_Q_MSG;


/*
 * typedef: struct APP_DEMO_ETH_PORT_STAT
 *
 * Description:
 *
 * Fields:
 *      pcktIn      - number of packets received counter
 *      pcktTreated - number of packets treated counter
 *      pcktLen     - number of packets with invalid length counter
 *      command     - number of packets with invalid Marvell-Tag command counter
 *      cpuCode     - number of packets with invalid Marvell-Tag CPU code counter
 *      noQbuffer   - lack of buffer resource counter
 *      msgSend     - message queue send failed counter
 *      intType     - invalid interrupt type counter
 *      device      - invalide device number (from Marvell-Tag) counter
 *
 * Comment:
 */
typedef struct
{
    GT_U32          pcktIn;
    GT_U32          pcktTreated;
    GT_U32          pcktLen;
    GT_U32          command;
    GT_U32          cpuCode;
    GT_U32          noQbuffer;
    GT_U32          msgSend;
    GT_U32          intType;
    GT_U32          device;
} APP_DEMO_ETH_PORT_STAT;

/* Rx/Tx queue message sizes */
#define APP_DEMO_Q_PTR_SIZE                     4
#define APP_DEMO_TX_PTR_NUM                     1
#define APP_DEMO_TX_Q_MSG_LEN                   (1 * APP_DEMO_Q_PTR_SIZE)
#define APP_DEMO_RX_Q_MSG_LEN                   (2 * APP_DEMO_Q_PTR_SIZE)

/* Header buffer size and offsets
 * Here are the pool buffer and original packet buffer offsets that are used
 * to build the first segment (buffer taken from pool) and the offset of the
 * original buffer to be used as the second segment.
 *
 *          first buffer          second buffer           second buffer
 *          taken from            original buffer         original buffer
 *          pool header           with VLAN tag           no VLAN tag
 *        _______________         _______________         _______________
 *       |  txDevice     |       |  MAC dest[0]  |       |  MAC dest[0]  |
 *       |  txQueue      |       |  MAC dest[1]  |       |  MAC dest[1]  |
 *       |   reserved    |       |  MAC dest[2]  |       |  MAC dest[2]  |
 *       |   reserved    |       |  MAC dest[3]  |       |  MAC dest[3]  |
 *       |   cookie[0]   |       |  MAC dest[4]  |       |  MAC dest[4]  |
 *       |   cookie[1]   |       |  MAC dest[5]  |       |  MAC dest[5]  |
 *       |   cookie[2]   |       |  MAC src[0]   |       |  MAC src[0]   |
 *       |   cookie[3]   |       |  MAC src[1]   |       |  MAC src[1]   |
 *       |   handle[0]   |       |  MAC src[2]   |       |  MAC src[2]   |
 *       |   handle[1]   |       |  MAC src[3]   |       |  MAC src[3]   |
 *       |   handle[2]   |       |  MAC src[4]   |       |  MAC src[4]   |
 *       |   handle[3]   |       |  MAC src[5]   |       |  MAC src[5]   |
 *       |  MAC dest[0]  |<-ptr  |     0x81      |       | EtherType/Len |<-ptr
 *       |  MAC dest[1]  |       |     0x00      |       | ngth field    |
 *       |  MAC dest[2]  |       |   802.1Q Tag  |       |    PAYLOAD    |
 *       |  MAC dest[3]  |       |   802.1Q Tag  |       |       "       |
 *       |  MAC dest[4]  |       | EtherType/Len |<-ptr  |       "       |
 *       |  MAC dest[5]  |       | ngth field    |       |       "       |
 *       |  MAC src[0]   |       |    PAYLOAD    |       -----------------
 *       |  MAC src[1]   |       |       "       |
 *       |  MAC src[2]   |       -----------------
 *       |  MAC src[3]   |
 *       |  MAC src[4]   |
 *       |  MAC src[5]   |
 *       |  MRVL tag[0]  |
 *       |  MRVL tag[1]  |
 *       |  MRVL tag[2]  |
 *       |  MRVL tag[3]  |
 *       -----------------
 */
#define APP_DEMO_ETHPRT_HDR_DEVICE_OFFSET       0 /* GT_U8 */
#define APP_DEMO_ETHPRT_HDR_QUEUE_OFFSET        1 /* GT_U8 */
#define APP_DEMO_ETHPRT_HDR_COOKIE_OFFSET       4
#define APP_DEMO_ETHPRT_HDR_HNDL_OFFSET         8
#define APP_DEMO_ETHPRT_HDR_MAC_SIZE            12
#define APP_DEMO_ETHPRT_HDR_MAC_OFFSET          12
#define APP_DEMO_ETHPRT_HDR_MRVLTAG_OFFSET      24
#define APP_DEMO_ETHPRT_HDR_SIZE                28

/* additional user packet header, this is used to indicate if the packet  */
/* was received tagged or not.                                            */
#define APP_DEMO_ETHPRT_TAGGED_PCKT_FLAG_LEN    4
#define APP_DEMO_ETHPRT_TAGGED_PCKT             1
#define APP_DEMO_ETHPRT_UNTAGGED_PCKT           2

#define APP_DEMO_ETHPRT_MRVLTAG_OFFSET          12
#define APP_DEMO_ETHPRT_VLANTAG_OFFSET          14
#define APP_DEMO_ETHPRT_BUFF_MIN_SIZE           60

/* offset in original packet for un/tagged frames */
#define APP_DEMO_BUFF_PCKT_UNTAGGED_OFFSET              12
#define APP_DEMO_BUFF_PCKT_TAGGED_OFFSET                16

#define GT_MRVL_TAG_SIZE                        4
#define GT_MRVL_TAG_PCKT_OFFSET                 12
#define GT_VLAN_ETHR_TYPE_MSB                   0x81
#define GT_VLAN_ETHR_TYPE_LSB                   0x00

/*
 * typedef: struct SW_TX_FREE_DATA_STCT
 *
 * Description: This structure is used when working in Event Request Driven mode
 *              The structure saves the transmitted packet cookie, device and
 *              queue number. The structure is a part of per application FIFO.
 *
 * Fields:  userData    - A data to be stored in gtBuf on packet transmit
 *                        request, and returned to user on txEnd.
 *          devNum      - The transmitted device number.
 *          queueNum    - The transmitted queue number.
 *          nextPtr     - The next packet data in the FIFO.
 *
 */
typedef struct _swTxFreeData
{
    GT_PTR                  userData;
    GT_U8                   devNum;
    GT_U8                   queueNum;
    struct _swTxFreeData    *nextPtr;
} SW_TX_FREE_DATA_STCT;


/*
 * typedef: struct GT_TX_END_FIFO
 *
 * Description: This structure is used when working in Event Request Driven mode.
 *              The structure implements a FIFO of transmitted packets.
 *
 * Fields:      headPtr - The head of the FIFO.
 *              tailPtr - The tail of the FIFO.
 *
 */
typedef struct
{
    SW_TX_FREE_DATA_STCT    *headPtr;
    SW_TX_FREE_DATA_STCT    *tailPtr;
} GT_TX_END_FIFO;

/*******************************************************************************
* appDemoEthPrtRxIntCauseGet
*
* DESCRIPTION:
*       This function check if the mrvl tag of the rx packet is for AU msg
*
* INPUTS:
*       devNum - dev num
*       mrvlTagPtr - (pointer to) the start of the mrvl tag in the packet
*
* OUTPUTS:
*       isAuMsgPtr - (pointer to) is the packet for AU message
*
* RETURNS:
*       GT_OK   - success
*       GT_FAIL - error
*
* COMMENTS:
*
*       function get all types of mrvl tag (regular , extended)
*
*******************************************************************************/
extern GT_STATUS appDemoEthPrtRxIntCauseGet
(
    IN GT_U8    devNum,
    IN  GT_U8   *mrvlTagPtr,
    OUT GT_BOOL *isAuMsgPtr
);

/*******************************************************************************
* appDemoEthPrtTxCtrlGet
*
* DESCRIPTION:
*       Return the ethernet port transmit control data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
APP_DEMO_ETH_PORT_TX_CTRL * appDemoEthPrtTxCtrlGet(GT_VOID);


/*******************************************************************************
* appDemoEthPrtRxCtrlGet
*
* DESCRIPTION:
*       Return the ethernet port recieve queue control data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
APP_DEMO_ETH_PORT_RX_Q_CTRL * appDemoEthPrtRxQCtrlGet(GT_VOID);


/*******************************************************************************
* appDemoEthPrtInitNetIfDev
*
* DESCRIPTION:
*       Initialize the network interface structures, Rx buffers and Tx header
*       buffers (For a single device).
*
* INPUTS:
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
GT_STATUS appDemoEthPrtInitNetIfDev
(
    void
);


/*******************************************************************************
* appDemoEthPrtTransmitPacket
*
* DESCRIPTION:
*       This function receives packet buffers & parameters from the different
*       core Tx functions. Prepares them for the transmit operation, and
*       enqueues the prepared descriptors to the PP's tx queues.
*
* INPUTS:
*       devNum -- dev num  -- the device to send the packet through
*       pcktParams  - The internal packet params to be set into the packet
*                     descriptors.
*       buffList    - The packet data buffers list.
*       buffLen     - A list of the buffers len in buffList.
*       numOfBufs   - Length of buffList.
*       portIsTagged - if the port is tagged member in VLAN
*       extPcktParams_PTR   - extended DSA packet parameters
*                           if extPcktParams_PTR == NULL
*                               engages regular DSA or network transmition
*                           else send with extended DSA tag
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NO_RESOURCE if there is not enough desc. for enqueuing the packet.
*       GT_ABORTED     if command aborted (during shutDown operation)
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*       *** support regular/extended Marvell Tag *****
*
*       common function for the :
*           1) build Eth port packet
*
*******************************************************************************/
GT_STATUS appDemoEthPrtTransmitPacket
(
    IN APP_DEMO_ETH_PORT_NET_TX_PARAMS_STC * pcktParams,
    IN GT_U8              * buffList[],
    IN GT_U32               buffLen[],
    IN GT_U32               numOfBufs
);

/*******************************************************************************
* appDemoEthPortTxEndGet
*
* DESCRIPTION:
*       This routine returns the caller the TxEnd parameters for a transmitted
*       packet.
*
* INPUTS:
*       hndl        - Handle got from gtEventBind.
*
* OUTPUTS:
*       devPtr      - The device the packet was transmitted from.
*       cookiePtr   - The user cookie handed on the transmit request.
*       queuePtr    - The queue from which this packet was transmitted
*       statusPtr   - GT_OK if packet transmission was successful, GT_FAIL on
*                     packet reject.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoEthPortTxEndGet
(
    IN GT_U32               hndl,
    OUT GT_U8               *devPtr,
    OUT GT_PTR              *cookiePtr,
    OUT GT_U8               *queuePtr,
    OUT GT_STATUS           *statusPtr
);


/*******************************************************************************
* appDemoEthPrtEthPrtRxPacketGet
*
* DESCRIPTION:
*       This function returns packets from PP destined to the CPU port.
*
* INPUTS:
*       queueIdx    - The queue from which this packet was received.
*       numOfBuffPtr- Num of buffs in packetBuffs.
*
* OUTPUTS:
*       numOfBuffPtr - Num of used buffs in packetBuffs.
*       packetBuffs  - The received packet buffers list.
*       buffLen      - List of buffer lengths for packetBuffs.
*       rxParamsPtr  - (Pointer to)information parameters of received packets
*
* RETURNS:
*       GT_NO_MORE  - no more packets on the device/queue
*       GT_OK       - packet got with no error.
*       GT_FAIL     - failed to get the packet
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum,portNum,queueIdx.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoEthPrtEthPrtRxPacketGet
(
    IN GT_U8                                queueIdx,
    INOUT GT_U32                            *numOfBuffPtr,
    OUT GT_U8                               *packetBuffs[],
    OUT GT_U32                              buffLen[],
    OUT APP_DEMO_ETH_PORT_NET_RX_PARAMS_STC *rxParamsPtr
);

/*******************************************************************************
* appDemoEthPrtRxBufFree
*
* DESCRIPTION:
*       Frees a list of buffers, that where previously passed to the upper layer
*       in an Rx event.
*
* INPUTS:
*       rxBuffList  - List of Rx buffers to be freed.
*       buffListLen - Length of rxBufList.
*       devNum      - The device number throw which these buffers where
*                     received.
*       rxQueue     - The Rx queue number throw which these buffers where
*                     received.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoEthPrtRxBufFree
(
    IN GT_U8    *rxBuffList[],
    IN GT_U32   buffListLen,
    IN GT_U8    rxQueue
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtAppDemoEthPrtCtrlh */


