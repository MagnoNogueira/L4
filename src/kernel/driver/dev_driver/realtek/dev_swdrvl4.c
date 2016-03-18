/* Module Name: DEV_SWDRVL4.C
 * Purpose:
 *      This package is a low level driver routine which provides the low level service routines
 *      to manage/drive the switch device.
 *      This package provides a generic interface to the upper layer SWDRV to manage the switch
 *      device for all layer 4 switch product lines.
 * Notes:
 *
 *
 *
 *
 * History:
 *       Date        Modifier    Reason
 *       2002/10/24   Dino King   Init for compile reference
 *       2002/12/20   Jeff Kao    Add Port Security Funciton
 *
 * Copyright(C)      Accton Corporation, 1999, 2000, 2001, 2002
 */

/* INCLUDE FILE DECLARATIONS
 */

#if 0
#include "sys_adpt.h"
#include "sys_dflt.h"
#include "dev_swdrvl4.h"
#include "dev_swdrvl4_pmgr.h"
#include "dev_swdrv.h"
#include "leaf_es3626a.h"
#include "stktplg_mgr.h"
#include "stktplg_pom.h"

#ifdef EIF8X10G
#include <soc/mcm/memregs.h>
#endif
#include <soc/mcm/allenum.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/devids.h>
#include <soc/drv.h>

#include <appl/diag/system.h>

#include <bcm/port.h>
#include <bcm/cosq.h>
#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/switch.h>

typedef struct
{
    BOOL_T  is_error;
    UI8_T   desc[40];

} DEV_SWDRVL4_BCM_ERR_DESC_T;

#if 0
static DEV_SWDRVL4_BCM_ERR_DESC_T dev_swdrvl4_bcm_error_list[20] =
{
    {FALSE, "DEV_SWDRVL4_BCM_E_NONE"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_INTERNAL"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_MEMORY"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_UNIT"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_PARAM"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_EMPTY"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_FULL"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_NOT_FOUND"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_EXISTS"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_TIMEOUT"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_BUSY"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_FAIL"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_DISABLED"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_BADID"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_RESOURCE"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_CONFIG"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_UNAVAIL"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_INIT"},
    {TRUE,  "DEV_SWDRVL4_BCM_E_PORT"}
};
#endif

/* for print out error msg*/
#define DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG(x) printf("%s(): Line = %d , %s\r\n", __FUNCTION__, __LINE__, (x));
#define DEV_SWDRVL4_CHIP_ERROR_MSG(message, error) printf("%s(): Line = %d , message=%ld, error_type=%d\r\n", __FUNCTION__, __LINE__, message, error);

#define DEV_SWDRVL4_PSEC_STATUS_ENABLE          0x1
#define DEV_SWDRVL4_PSEC_SHUTDOWN_STATUS_ENABLE 0x2
#define DEV_SWDRVL4_PSEC_TRAP_STATUS_ENABLE     0x4

#ifdef EIF8X10G
/* XGS 5670 queue mode
 */
#define DEV_SWDRVL4_HERCULES_COSQ_STRICT_SCHEDULE           0x0
#define DEV_SWDRVL4_HERCULES_COSQ_WRR_SCHEDULE              0x1
#define DEV_SWDRVL4_MAX_DEVICE_CONNECTED_TO_HERCULES        8
#define DEV_SWDRVL4_HERCULES_DEVICE_ID                      0
#endif

/* Merge the port security <enable/disable>,
 *           port security active shutdown <enable/disable>,
 *           port security active trap <enable/disable>
 */
static uint32  psec_status_bmp[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT][SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP];
//static uint32  psec_status_bmp[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT][SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP]={{NULL}};
#if 0
static int temp_weight[BCM_COS_COUNT]={1,1,1,1,1,1,1,1};
#endif

static BOOL_T DEV_SWDRVL4_TrafficSegmatationStatus = TRUE;

static BOOL_T static_move_to_cpu_shadow = FALSE;

typedef struct
{
    int queue_id[SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE]; /* per queue weight */
} DEV_SWDRVL4_WRR_QUEUE_T;

static SYS_TYPE_Uport_T uplink_uport_list[SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT * SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];
static SYS_TYPE_Uport_T downlink_uport_list[SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT * SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK];
static BOOL_T DEV_SWDRVL4_GetNextIndexFromPortList (UI32_T *index,
                                                    UI8_T  port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST]);
#define DEV_SWDRVL4_IFINDEX_TO_UNIT(ifindex)    ( ((UI32_T)((ifindex-1)/SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT))+1 )
#define DEV_SWDRVL4_IFINDEX_TO_PORT(ifindex)    ( ifindex - (DEV_SWDRVL4_IFINDEX_TO_UNIT(ifindex)-1)*SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT )

static DEV_SWDRVL4_WRR_QUEUE_T swdrvl4_wrr_queue[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT][SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP];

/* Default Burst Size for Token Bucket : kbits */
/* XGS II Family Chip */
#define XGS_DRACO_DEFAULT_METER_BURST_SIZE    524    /* 524K bits */
#define XGS_LYNX_DEFAULT_METER_BURST_SIZE     64000  /* 64M  bits */
#define XGS_TUCANA_DEFAULT_METER_BURST_SIZE   512    /* 512K bits */

/* XGS III Family Chip */
#define XGS_XGS_III_DEFAULT_METER_BURST_SIZE  128000 /* 128M bits */

static int DEV_SWDRVL4_SetStaticMacPortMoveNotification(BOOL_T is_enable);
static BOOL_T DEV_SWDRVL4_CheckAllPortSecActionDisabled(UI32_T ignore_device_id, UI32_T ignore_phy_port);

/******************* Default values of Ingress CoS to internal DSCP Mapping Table ****************/
const static dev_swdrvl4_internal_dscp_t DEV_SWDRVL4_DEFAULT_COS_TO_DSCP_MAPPING[DEV_SWDRVL4_INGRESS_COS_TO_DSCP_MAPPING_ENTRY_NMB][DEV_SWDRVL4_INGRESS_COS_TO_DSCP_MAPPING_CFI] = {
           {{ 0, 0}, { 0, 0}},
           {{ 1, 0}, { 1, 0}},
               {{ 2, 0}, { 2, 0}},
               {{ 3, 0}, { 3, 0}},
               {{ 4, 0}, { 4, 0}},
               {{ 5, 0}, { 5, 0}},
               {{ 6, 0}, { 6, 0}},
               {{ 7, 0}, { 7, 0}}

};

/******************* Default values of internal DSCP to CoS Mapping Table ****************/
const static dev_swdrvl4_outer_cos_t  DEV_SWDRVL4_DEFAULT_DSCP_TO_COS_MAPPING[DEV_SWDRVL4_INGRESS_DSCP_TO_COS_MAPPING_PHB*2][DEV_SWDRVL4_INGRESS_DSCP_TO_COS_MAPPING_COLOR] = {
            {{0,0},{0,0},{0,0},{0,0}},
            {{0,0},{0,0},{0,0},{0,0}},
            {{1,0},{1,0},{1,0},{1,0}},
            {{1,0},{1,0},{1,0},{1,0}},
            {{2,0},{2,0},{2,0},{2,0}},
            {{2,0},{2,0},{2,0},{2,0}},
            {{3,0},{3,0},{3,0},{3,0}},
            {{3,0},{3,0},{3,0},{3,0}},
            {{4,0},{4,0},{4,0},{4,0}},
            {{4,0},{4,0},{4,0},{4,0}},
            {{5,0},{5,0},{5,0},{5,0}},
            {{5,0},{5,0},{5,0},{5,0}},
            {{6,0},{6,0},{6,0},{6,0}},
            {{6,0},{6,0},{6,0},{6,0}},
            {{7,0},{7,0},{7,0},{7,0}},
            {{7,0},{7,0},{7,0},{7,0}}

};

/******************* Default values of Ingress DSCP to internal DSCP Mapping Table ****************/
const static dev_swdrvl4_internal_dscp_t DEV_SWDRVL4_DEFAULT_DSCP_TO_DSCP_MAPPING[DEV_SWDRVL4_INGRESS_DSCP_TO_DSCP_MAPPING_ENTRY_NMB] = {
            {0,0},{0,1},{0,0},{0,3},{0,0},{0,1},{0,0},{0,3},
            {1,0},{1,1},{1,0},{1,3},{1,0},{1,1},{1,0},{1,3},
            {2,0},{2,1},{2,0},{2,3},{2,0},{2,1},{2,0},{2,3},
            {3,0},{3,1},{3,0},{3,3},{3,0},{3,1},{3,0},{3,3},
            {4,0},{4,1},{4,0},{4,3},{4,0},{4,1},{4,0},{4,3},
            {5,0},{5,1},{5,0},{5,3},{5,0},{5,1},{5,0},{5,3},
            {6,0},{6,1},{6,0},{6,3},{6,0},{6,1},{6,0},{6,3},
            {7,0},{7,1},{7,0},{7,3},{7,0},{7,1},{7,0},{7,3}
};

/******************* Default values of internal Priority to Queue Mapping Table ****************/
const static UI8_T DEV_SWDRVL4_DEFAULT_DSCP_TO_QUEUE_MAPPING[DEV_SWDRVL4_DSCP_TO_QUEUE_MAPPING_ENTRY_NMB] = {
            (UI8_T)DEV_SWDRVL4_DFLT_PRI_0_TO_QUEUE,
            (UI8_T)DEV_SWDRVL4_DFLT_PRI_1_TO_QUEUE,
            (UI8_T)DEV_SWDRVL4_DFLT_PRI_2_TO_QUEUE,
            (UI8_T)DEV_SWDRVL4_DFLT_PRI_3_TO_QUEUE,
            (UI8_T)DEV_SWDRVL4_DFLT_PRI_4_TO_QUEUE,
            (UI8_T)DEV_SWDRVL4_DFLT_PRI_5_TO_QUEUE,
            (UI8_T)DEV_SWDRVL4_DFLT_PRI_6_TO_QUEUE,
            (UI8_T)DEV_SWDRVL4_DFLT_PRI_7_TO_QUEUE,
};

#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE)
#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION)
/* for traffic segmentation
 */
static struct
{
    /* shadow of portlist
     */
    pbmp_t normal_egr_pbmp[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];
    struct{
        pbmp_t uplink_egr_pbmp[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];
        pbmp_t downlink_egr_pbmp[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];
    } session_info[SYS_ADPT_PORT_TRAFFIC_SEGMENTATION_MAX_NBR_OF_SESSIONS];

    /* TRUE  - up-to-up is block
     * FALSE - up-to-up is forward
     */
    BOOL_T is_blocking_uplink_group;

    /* normal port: port_session_id == 0
     * uplink port: port_session_id != 0 && uplink_list != 0
     * downlink port: port_session_id != 0 && uplink_list == 0
     */
    UI32_T port_session_id[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK][SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT];
    UI8_T uplink_list[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK * SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];
    UI32_T trunk_session_id[SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM];
    BOOL_T trunk_is_uplink[SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM];
}
dev_swdrvl4_traffic_seg;
#endif
#endif


/* 2008-07-22, Jinfeng Chen:
   never used, hide it to remove warning when compiling.
 */

#if 0
SYSFUN_DECLARE_CSC
#endif
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_Init
 *------------------------------------------------------------------------------
 * PURPOSE : This function allocates the system resource for low level switch l4
 *           driver initiation.
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    :
 *------------------------------------------------------------------------------
 */
void DEV_SWDRVL4_Init()
{
    memset(psec_status_bmp, 0, sizeof(psec_status_bmp));
    memset(swdrvl4_wrr_queue, 0, sizeof(swdrvl4_wrr_queue));
} /* end of DEV_SWDRVL4_Init() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_Create_InterCSC_Relation
 *------------------------------------------------------------------------------
 * FUNCTION: This function initializes all function pointer registration operations.
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    : None
 *------------------------------------------------------------------------------*/
void DEV_SWDRVL4_Create_InterCSC_Relation(void)
{
    return;
}

static BOOL_T DEV_SWDRVL4_SetCosTrustModeStub(UI32_T device_id, UI32_T phy_port, UI32_T mode)
{
    int error;

    if ((SOC_IS_RAVEN(device_id)) || (SOC_IS_FIREBOLT2(device_id)))
    {
    error = bcm_port_dscp_map_mode_set(device_id,phy_port, mode);
    }
    else if (SOC_IS_FIREBOLT(device_id))
    {
        error = bcm_port_dscp_map_mode_set(device_id,-1, mode);
    }

    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
        return FALSE;
    }

    error = bcm_switch_control_port_set(device_id,phy_port, bcmSwitchColorSelect,1);
    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
        return FALSE;
    }

    error = bcm_switch_control_port_set(device_id,phy_port, bcmSwitchColorSelect,2);
    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
        return FALSE;
    }
    return TRUE;
}


/*=====================================================================
 * Exported functions for COS
 *=====================================================================
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_Init
 *------------------------------------------------------------------------------
 * PURPOSE : This function allocates the system resource for low level switch l4
 *           driver initiation.
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * NOTE    :
 *------------------------------------------------------------------------------
 */
BOOL_T DEV_SWDRVL4_Cos_Init(int device_id, int phy_port)
{
#if (SYS_CPNT_COS_INTER_DSCP == TRUE)
    int  i,j,error;
    bcm_cos_queue_t  temp_queue;
    bcm_cos_t        priority;
    color_t          temp_color;
    UI32_T   color = 0;
    int srccp, drccp, prio;

    //write the Cos tables of the chip
    /*replace bcm_port_dscp_map_mode_set to set color selection*/
    if(!DEV_SWDRVL4_SetCosTrustModeStub(device_id, phy_port, 0))
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, 0);
        return FALSE;
    }

    for(i = 0; i <= DEV_SWDRVL4_MAX_PRE_VAL; i++)
    {
        priority  = (bcm_cos_t)i;
        temp_queue = (bcm_cos_queue_t)DEV_SWDRVL4_DEFAULT_DSCP_TO_QUEUE_MAPPING[i];
        error = bcm_cosq_port_mapping_set(device_id,phy_port,priority,temp_queue);
        if (error != BCM_E_NONE)
        {
            DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
            return FALSE;
        }
    }

    for(i = 0; i <= DEV_SWDRVL4_MAX_COS_VAL; i++)
    {
        for(j= 0;j<2;j++)
        {
            temp_color = (color_t)DEV_SWDRVL4_DEFAULT_COS_TO_DSCP_MAPPING[i][j].color;
            if(temp_color == colorGreen)
            {
                color = bcmColorGreen;
            }
            else if(temp_color == colorYellow)
            {
                color = bcmColorYellow;
            }
            else if(temp_color == colorRed)
            {
                color = bcmColorRed;
            }

            if ((SOC_IS_RAVEN(device_id)) || (SOC_IS_FIREBOLT2(device_id)))
            {
                error = bcm_port_vlan_priority_map_set(device_id,phy_port,i, j,DEV_SWDRVL4_DEFAULT_COS_TO_DSCP_MAPPING[i][j].phb,color);
            }
            else if (SOC_IS_FIREBOLT(device_id))
            {
                error = bcm_port_cfi_color_set(device_id, phy_port, j, color);
            }

            if (error != BCM_E_NONE)
            {
                DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
                return FALSE;
            }
        }
    }

    for(i = 0; i <= (DEV_SWDRVL4_MAX_PHB_VAL*2+1); i++)
    {
        for(j=0;j<4;j++)
        {
            temp_color = (color_t)j;
            if(temp_color == colorGreen)
            {
                color = bcmColorGreen;
            }
            else if(temp_color == colorYellow)
            {
                color = bcmColorYellow;
            }
            else if(temp_color == colorRed)
            {
                color = bcmColorRed;
            }

            if ((SOC_IS_RAVEN(device_id)) || (SOC_IS_FIREBOLT2(device_id)))
            {
                error = bcm_port_vlan_priority_unmap_set(device_id,phy_port,i/2,color,DEV_SWDRVL4_DEFAULT_DSCP_TO_COS_MAPPING[i][j].cos, DEV_SWDRVL4_DEFAULT_DSCP_TO_COS_MAPPING[i][j].cfi);
            }
            if (error != BCM_E_NONE)
            {
                DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
                return FALSE;
            }
        }
    }

    for(i=0;i<=DEV_SWDRVL4_MAX_PHB_VAL;i++)
    {
        color = bcmColorGreen;
        error = bcm_port_priority_color_set (device_id,phy_port,i, color);
        if (error != BCM_E_NONE)
        {
            DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
            return FALSE;
        }
    }

    for(i = 0; i <= DEV_SWDRVL4_MAX_DSCP_VAL; i++)
    {
        srccp = i;
        temp_color  = DEV_SWDRVL4_DEFAULT_DSCP_TO_DSCP_MAPPING[i].color;
        prio =  (int)DEV_SWDRVL4_DEFAULT_DSCP_TO_DSCP_MAPPING[i].phb;
        drccp = (DEV_SWDRVL4_DEFAULT_DSCP_TO_DSCP_MAPPING[i].phb<<3) + DEV_SWDRVL4_DEFAULT_DSCP_TO_DSCP_MAPPING[i].color;

        if(temp_color ==colorRed)
        {
            prio = (prio |0x100);
        }
        if(temp_color ==colorYellow)
        {
            prio = (prio |0x200);
        }

        if (soc_feature(device_id, soc_feature_dscp_map_per_port))
        {
            error = bcm_port_dscp_map_set( device_id,phy_port,srccp,drccp, prio);
        }
        else
        {
            error = bcm_port_dscp_map_set( device_id,-1,srccp,drccp, prio);
        }

        if (error != BCM_E_NONE)
        {
            DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
            return FALSE;
        }
    }
#endif /* #if (SYS_CPNT_COS_INTER_DSCP == TRUE) */
        return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnableCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function globally enables the specific COS mapping
 * INPUT   : maptype -- which type of mapping is being enabled
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : 1. If the system support per port enable, this API
 *              enables the specific mapping of all ports
 *           2. This API may need to config the ASIC if the mapping
 *              is changed from disable to enable.
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_EnableCosMapping(DEV_SWDRVL4_CosMapType_T maptype)
{
    UI32_T  device_id;
    int     error;

    for (device_id=0; device_id<soc_ndev; device_id++)
    {
        if ((maptype == DEV_SWDRVL4_MapTosToCos) || (maptype == DEV_SWDRVL4_MapDscpToCos))
        {
            /* Enable TRUST_DSCP_V4 & TRUST_DSCP_V6 in port_tab. */
            error = bcm_port_dscp_map_mode_set(device_id, -1, BCM_PORT_DSCP_MAP_ALL);
            if (error != BCM_E_NONE)
            {
                DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
                return FALSE;
            }
        }
        else
        {
            /* don't support */
            return FALSE;
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnablePortCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function per port enables the specific COS mapping
 * INPUT   : maptype -- which type of mapping is being enabled
 *           unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : 1. If the system soes not support per port enable/disable,
 *              this API always returns false
 *           2. Module_ID will be translated from unit, port by device driver
 *              The DevicePortMapping table will be passed down from core layer
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_EnablePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                        UI32_T unit,
                                        UI32_T port)
{
    UI32_T  phy_port, device_id, module_id;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_EnablePortCosMapping");
        return FALSE;
    }

    if ((maptype == DEV_SWDRVL4_MapTosToCos) || (maptype == DEV_SWDRVL4_MapDscpToCos))
    {
        /* do nothing ; when config rate's value , it will enable maptype
         */
        return TRUE;
    }

    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DisableCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function globally disables the specific COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : 1. If the system support per port disable, this API
 *              disables the specific mapping of all ports
 *           2. This API may need to config the ASIC if the mapping
 *              is changed from disable to disable.
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DisableCosMapping(DEV_SWDRVL4_CosMapType_T maptype)
{
    UI32_T  device_id;
    int     error;

    for (device_id=0; device_id<soc_ndev; device_id++)
    {
        if ((maptype == DEV_SWDRVL4_MapTosToCos) || (maptype == DEV_SWDRVL4_MapDscpToCos))
        {
            /* Disable TRUST_DSCP_V4 & TRUST_DSCP_V6 in port_tab. */
            error = bcm_port_dscp_map_mode_set(device_id, -1, BCM_PORT_DSCP_MAP_NONE);
            if (error != BCM_E_NONE)
            {
                DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
                return FALSE;
            }
        }
        else
        {
            /* don't support */
            return FALSE;
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DisablePortCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function per port disables the specific COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : If the system soes not support per port disable/disable,
 *           this API always returns false
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DisablePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                         UI32_T unit,
                                         UI32_T port)
{
    UI32_T  phy_port, device_id, module_id, cos=0;
    int     error;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_SetPortCosMapping");
        return FALSE;
    }

    if (!soc_feature(device_id, soc_feature_dscp_map_per_port))
    {
        phy_port = -1;
    }

    if ((maptype == DEV_SWDRVL4_MapTosToCos) || (maptype == DEV_SWDRVL4_MapDscpToCos))
    {
        /* -1 : mean don't remap dscp value */
        error = bcm_port_dscp_map_set(device_id, phy_port, -1, -1, cos);
        if (error != BCM_E_NONE)
        {
            DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }


    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function globally sets the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with global enable
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                 UI32_T value,
                                 UI32_T cos)
{
    UI32_T  device_id, i;
    int     error;

    for (device_id=0; device_id<soc_ndev; device_id++)
    {
         if (maptype == DEV_SWDRVL4_MapTosToCos)
         {
            for (i=1; i<=8; i++)
            {
                error = bcm_port_dscp_map_set(device_id, -1, value*i, value*i, cos);
                if (error != BCM_E_NONE)
                {
                    DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
                    return FALSE;
                }
            }
        }
        else if (maptype == DEV_SWDRVL4_MapDscpToCos)
        {
            error = bcm_port_dscp_map_set(device_id, -1, value, value, cos);
            if (error != BCM_E_NONE)
            {
                DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
                return FALSE;
            }
        }
        else
        {
            /* only support TOS/DSCP value */
            DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_SetPortCosMapping");
            return FALSE;
        }
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function per port sets the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           unit -- physically stacking unit(box) number
 *           port: user port in this unit
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with per port enable
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                     UI32_T unit,
                                     UI32_T port,
                                     UI32_T value,
                                     UI32_T cos)
{
    UI32_T  phy_port, device_id, module_id, i;
    int     error;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_SetPortCosMapping");
        return FALSE;
    }

    if (!soc_feature(device_id, soc_feature_dscp_map_per_port))
    {
        phy_port = -1;
    }

    if (maptype == DEV_SWDRVL4_MapTosToCos)
    {
        for (i=1; i<=8; i++)
        {
            error = bcm_port_dscp_map_set(device_id, phy_port, value*i, value*i, cos);
            if (error != BCM_E_NONE)
            {
                DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
                return FALSE;
            }
        }
    }
    else if (maptype == DEV_SWDRVL4_MapDscpToCos)
    {
        error = bcm_port_dscp_map_set(device_id, phy_port, value, value, cos);
        if (error != BCM_E_NONE)
        {
            DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
            return FALSE;
        }
    }
    else
    {
        /* only support TOS/DSCP value */
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_SetPortCosMapping");
        return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DeleteCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function globally deletes the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with global disable
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DeleteCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                    UI32_T value,
                                    UI32_T cos)
{
    UI32_T  device_id, i;
    int     error;

    for (device_id=0; device_id<soc_ndev; device_id++)
    {
        if (maptype == DEV_SWDRVL4_MapTosToCos)
        {
            for (i=1; i<=8; i++)
            {
                error = bcm_port_dscp_map_set(device_id, -1, value*i, value*i, cos);
                if (error != BCM_E_NONE)
                {
                    DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
                    return FALSE;
                }
            }
         }
         else if (maptype == DEV_SWDRVL4_MapDscpToCos)
         {
            error = bcm_port_dscp_map_set(device_id, -1, value, value, cos);
            if (error != BCM_E_NONE)
            {
                DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
                return FALSE;
            }
         }
         else
         {
             /* only support TOS/DSCP value */
             DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_SetPortCosMapping");
             return FALSE;
         }
     }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DeletePortCosMapping
 *------------------------------------------------------------------------------
 * FUNCTION: This function per port deletes the COS mapping
 * INPUT   : maptype -- which type of mapping is being disabled
 *           unit -- physically stacking unit(box) number
 *           port: user port in this unit
 *           value: TOS/DSCP/TCP_Port/UDP_Port value to be mapped
 *           cos :  user priority (logical cos value)
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : similar with per port disable
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DeletePortCosMapping(DEV_SWDRVL4_CosMapType_T maptype,
                                        UI32_T unit,
                                        UI32_T port,
                                        UI32_T value,
                                        UI32_T cos)
{
    UI32_T  phy_port, device_id, module_id, i;
    int     error;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_DeletePortCosMapping");
        return FALSE;
    }

    if (!soc_feature(device_id, soc_feature_dscp_map_per_port))
    {
        phy_port = -1;
    }

    if (maptype == DEV_SWDRVL4_MapTosToCos)
    {
        for (i=1; i<=8; i++)
        {
            error = bcm_port_dscp_map_set(device_id, phy_port, value*i, value*i, cos);
            if (error != BCM_E_NONE)
            {
                DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
                return FALSE;
            }
        }
    }
    else if (maptype == DEV_SWDRVL4_MapDscpToCos)
    {
        error = bcm_port_dscp_map_set(device_id, phy_port, value, value, cos);
        if (error != BCM_E_NONE)
        {
            DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
            return FALSE;
        }
    }
    else
    {
        /* only support TOS/DSCP value */
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_DeletePortCosMapping");
        return FALSE;
    }

    return TRUE;
}

/*=====================================================================
 * Exported functions for Rate-Limit
 *=====================================================================
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnablePortIngressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: his function per port enables the ingress rate limit
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : Module_ID will be translated from unit, port by device driver
 *           The DevicePortMapping table will be passed down from core layer
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_EnablePortIngressRateLimit(UI32_T unit,
                                              UI32_T port)
{
    UI32_T  phy_port, device_id, module_id;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_EnablePortIngressRateLimit");
        return FALSE;
    }

    /* do nothing ; when config rate's value , it will enable rate limit
     */
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DisablePortIngressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: his function per port disables the ingress rate limit
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 * OUTPUT  : None
 * RETURN  : TRUE for success and FALSE for failure
 * NOTE    : Module_ID will be translated from unit, port by device driver
 *           The DevicePortMapping table will be passed down from core layer
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DisablePortIngressRateLimit(UI32_T unit,
                                              UI32_T port)
{
    int     error;
    UI32_T  phy_port, device_id, module_id;
    uint32  burst=0, rate=0;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_DisablePortIngressRateLimit");
        return FALSE;
    }

    error = bcm_port_rate_ingress_set(device_id, phy_port, rate, burst);
    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
        return FALSE;
    }
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortIngressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets the port ingress rate limit
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- which port to set
 *           rate -- port ingress rate limit
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : 1. The limit is the total limitation for the traffic received
 *              from the specific port
 *           2. For Strata switch, the grain of 125KBytes for EPIC,
 *              and 8MByes for GPIC
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortIngressRateLimit(UI32_T unit,
                                           UI32_T port,
                                           UI32_T rate)
{
    int     error;
    UI32_T  phy_port, device_id, module_id;
    uint32  burst;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_SetPortIngressRateLimit");
        return FALSE;
    }
#if 0
    if (SOC_IS_DRACO(device_id))
        burst = XGS_DRACO_DEFAULT_METER_BURST_SIZE;
    else if (SOC_IS_TUCANA(device_id))
        burst = XGS_TUCANA_DEFAULT_METER_BURST_SIZE;
    else if (SOC_IS_LYNX(device_id))
        burst = XGS_LYNX_DEFAULT_METER_BURST_SIZE;
    else if (SOC_IS_HELIX(device_id) || SOC_IS_FIREBOLT(device_id) || SOC_IS_EASYRIDER(device_id) ||
             SOC_IS_RAPTOR(device_id) || SOC_IS_FIREBOLT2(device_id))
#endif
        burst = SYS_DFLT_RATE_LIMIT_DFLT_BURST_KBYTES * 8;

    error = bcm_port_rate_ingress_set(device_id, phy_port, rate, burst);
    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
        return FALSE;
    }
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnablePortEgressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: This function enables the port egress rate limit
 * INPUT   : unit -- device_id
 *           port -- phy_port
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_EnablePortEgressRateLimit(UI32_T unit, UI32_T port)
{
    UI32_T  phy_port, device_id, module_id;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_EnablePortEgressRateLimit");
        return FALSE;
    }

    /* do nothing ; when config rate's value , it will enable rate limit
     */
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DisablePortEgressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: This function disables the port egress rate limit
 * INPUT   : unit -- device_id
 *           port -- phy_port
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DisablePortEgressRateLimit(UI32_T unit, UI32_T port)
{
    int     error;
    UI32_T  phy_port, device_id, module_id;
    uint32  burst=0, rate=0;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_SetPortEgressRateLimit");
        return FALSE;
    }

    error = bcm_port_rate_egress_set(device_id, phy_port, rate, burst);
    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
        return FALSE;
    }
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortEgressRateLimit
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets the port gress rate limit
 * INPUT   : unit -- device_id
 *           port -- phy_port
 *           rate -- port egress rate limit
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortEgressRateLimit(UI32_T unit, UI32_T port, UI32_T rate)
{
    int     error;
    UI32_T  phy_port, device_id, module_id;
    uint32  burst;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_SetPortEgressRateLimit");
        return FALSE;
    }

#if 0
    if (SOC_IS_DRACO(device_id))
        burst = XGS_DRACO_DEFAULT_METER_BURST_SIZE;
    else if (SOC_IS_TUCANA(device_id))
        burst = XGS_TUCANA_DEFAULT_METER_BURST_SIZE;
    else if (SOC_IS_LYNX(device_id))
        burst = XGS_LYNX_DEFAULT_METER_BURST_SIZE;
    else if (SOC_IS_HELIX(device_id) || SOC_IS_FIREBOLT(device_id) || SOC_IS_EASYRIDER(device_id) ||
             SOC_IS_RAPTOR(device_id) || SOC_IS_FIREBOLT2(device_id))
#endif
        burst = SYS_DFLT_RATE_LIMIT_DFLT_BURST_KBYTES * 8;

    error = bcm_port_rate_egress_set(device_id, phy_port, rate, burst);
    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
        return FALSE;
    }
    return TRUE;
}


/*=====================================================================
 * Exported functions for Egress Scheduling
 *=====================================================================
 */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetEgressSchedulingMethod
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets the scheduling method for egress queues
 * INPUT   : method -- scheduling method, currently support two methods
 *                     DEV_SWDRVL4_WeightedFairRoundRobinScheduling
 *                     DEV_SWDRVL4_StrictPriorityScheduling
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : For Strata Switch, default is StrictPriorityScheduling
 *           This function is for global configuration
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetEgressSchedulingMethod(DEV_SWDRVL4_EgressSchedulingMethod_T method)
{
    int     error=0;
    UI32_T  device_id;

#ifdef EIF8X10G
    UI32_T phy_port;
#endif
    /* Check if method is either DEV_SWDRVL4_WeightedFairRoundRobinScheduling
     * or DEV_SWDRVL4_StrictPriorityScheduling, which is currently supported.
     */
    if( (method != DEV_SWDRVL4_WeightedFairRoundRobinScheduling) &&
        (method != DEV_SWDRVL4_StrictPriorityScheduling) &&
        (method != DEV_SWDRVL4_WeightedRoundRobinScheduling) &&
        (method != DEV_SWDRVL4_DeficitRoundRobinScheduling) &&
        (method != DEV_SWDRVL4_SpWithDrrScheduling) &&
        (method != DEV_SWDRVL4_SpWithWrrScheduling))
    {
        return FALSE;
    }

    for (device_id=0; device_id < soc_ndev; device_id++)
    {
#if 0
        if (SOC_IS_TUCANA(device_id) || SOC_IS_DRACO(device_id)    || SOC_IS_LYNX(device_id) ||
            SOC_IS_HELIX(device_id)  || SOC_IS_FIREBOLT(device_id) || SOC_IS_EASYRIDER(device_id) ||
            SOC_IS_RAPTOR(device_id))
        {
            if (method == DEV_SWDRVL4_WeightedFairRoundRobinScheduling)
            {
#if 0 /* squid */
                PBMP_E_ITER(device_id, phy_port)
                {
                    error = bcm_cosq_port_sched_set(device_id, SOC_BLOCK_BITMAP(device_id, SOC_PORT_BLOCK(device_id, phy_port)), BCM_COSQ_WEIGHTED_ROUND_ROBIN, swdrvl4_wrr_queue[device_id][phy_port].queue_id, 0);
                }
#endif
                error = bcm_cosq_port_sched_set(device_id, PBMP_E_ALL(device_id), BCM_COSQ_WEIGHTED_ROUND_ROBIN, swdrvl4_wrr_queue[0][0].queue_id, 0);

            }
            else /* if(method == DEV_SWDRVL4_StrictPriorityScheduling) */
            {
                error = bcm_cosq_port_sched_set(device_id, PBMP_E_ALL(device_id), BCM_COSQ_STRICT, swdrvl4_wrr_queue[0][0].queue_id, 0);
            }

            if (error != 0)
                return FALSE;
        }
        if(SOC_IS_FIREBOLT2(device_id))
#endif
        {
            switch(method)
            {
                case DEV_SWDRVL4_StrictPriorityScheduling :
                    error = bcm_cosq_port_sched_set(device_id, PBMP_E_ALL(device_id), BCM_COSQ_STRICT, swdrvl4_wrr_queue[0][0].queue_id, 0);
                    break;
                case DEV_SWDRVL4_SpWithWrrScheduling :
                case DEV_SWDRVL4_WeightedRoundRobinScheduling :
                    error = bcm_cosq_port_sched_set(device_id, PBMP_E_ALL(device_id), BCM_COSQ_WEIGHTED_ROUND_ROBIN, swdrvl4_wrr_queue[0][0].queue_id, 0);
                    break;
                case DEV_SWDRVL4_SpWithDrrScheduling :
                case DEV_SWDRVL4_DeficitRoundRobinScheduling :
                    error = bcm_cosq_port_sched_set(device_id, PBMP_E_ALL(device_id), BCM_COSQ_DEFICIT_ROUND_ROBIN, swdrvl4_wrr_queue[0][0].queue_id, 0);
                    break;
                case DEV_SWDRVL4_WeightedFairRoundRobinScheduling :
                default:
                    break;
            }
            if(0 != error)
                return FALSE;
        }
#ifdef EIF8X10G
        if(SOC_IS_HERCULES(device_id))
        {
            if (method == DEV_SWDRVL4_WeightedFairRoundRobinScheduling)
            {
/*
PRBOBLEM:cannot find stacking port in BT
ROOT CAUSE:it get stacking port by hg,but bt doesn't have hg
SOLUTION:get stacking port by stacking info
Files:dev_swdrvl4.c,dev_swdrv.c,dev_amtrdrv.c
*/

#if 0
                PBMP_ITER(PBMP_HG_ALL(device_id), phy_port)
#else/*make the code comm*/
                PBMP_ITER(PBMP_ST_ALL(device_id), phy_port)
#endif
                {
                    WRITE_MMU_EGS_PRIMODr(device_id, phy_port, DEV_SWDRVL4_HERCULES_COSQ_WRR_SCHEDULE);
                }
            }
            else /* if(method == DEV_SWDRVL4_StrictPriorityScheduling) */
            {
/*
PRBOBLEM:cannot find stacking port in BT
ROOT CAUSE:it get stacking port by hg,but bt doesn't have hg
SOLUTION:get stacking port by stacking info
Files:dev_swdrvl4.c,dev_swdrv.c,dev_amtrdrv.c
*/

#if 0
                 PBMP_ITER(PBMP_HG_ALL(device_id), phy_port)
#else/*make the code comm*/
                 PBMP_ITER(PBMP_ST_ALL(device_id), phy_port)
#endif

                {
                    WRITE_MMU_EGS_PRIMODr(device_id, phy_port, DEV_SWDRVL4_HERCULES_COSQ_STRICT_SCHEDULE);
                }
            }
        }
#endif
    }

    return TRUE;
}

/*=====================================================================
 * Exported functions for Egress Scheduling per port
 *=====================================================================
 */
/*------------------------------------------------------------------------------
  ROUTINE NAME - DEV_SWDRVL4_SetPortEgressSchedulingMethod
  -------------------------------------------------------------------------
  FUNCTION: This function sets the scheduling method for egress queues per port
  INPUT   :     unit -- which unit to set
                port -- which port to set
                method -- scheduling method, currently support two methods
                        DEV_SWDRVL4_WeightedFairRoundRobinScheduling
                        DEV_SWDRVL4_StrictPriorityScheduling
                        DEV_SWDRVL4_WeightedRoundRobinScheduling
                        DEV_SWDRVL4_DeficitRoundRobinScheduling
                        DEV_SWDRVL4_DeficitRoundRobinScheduling
                        DEV_SWDRVL4_SpWithWrrScheduling
  OUTPUT  : None
  RETURN  : True: Successfully, FALSE: Failed
  NOTE    : For Strata Switch, default is StrictPriorityScheduling
            This function is for individual configuration
  -------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortEgressSchedulingMethod(UI32_T unit, UI32_T port, DEV_SWDRVL4_EgressSchedulingMethod_T method)
{
    UI32_T error=0;
    UI32_T device_id, my_module_id, phy_port;
        bcm_pbmp_t pbmp;

    if( (method != DEV_SWDRVL4_StrictPriorityScheduling) &&
        (method != DEV_SWDRVL4_WeightedRoundRobinScheduling) &&
        (method != DEV_SWDRVL4_DeficitRoundRobinScheduling) &&
        (method != DEV_SWDRVL4_SpWithDrrScheduling) &&
        (method != DEV_SWDRVL4_SpWithWrrScheduling))
    {
        return FALSE;
    }
        DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &my_module_id, &device_id, &phy_port);

//      if(SOC_IS_FIREBOLT2(device_id))
        {
                UI32_T i;
                int dflt_weights[SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE] =
                                {       SYS_DFLT_WRR_Q0_WEIGHT,
                                        SYS_DFLT_WRR_Q1_WEIGHT,
                                        SYS_DFLT_WRR_Q2_WEIGHT,
                                        SYS_DFLT_WRR_Q3_WEIGHT,
                                #if (8 == SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE)
                                        SYS_DFLT_WRR_Q4_WEIGHT,
                                        SYS_DFLT_WRR_Q5_WEIGHT,
                                        SYS_DFLT_WRR_Q6_WEIGHT,
                                        SYS_DFLT_WRR_Q7_WEIGHT,
                                #endif
                                };
                int zero_weights[SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE];

                memset(zero_weights, 0, sizeof(zero_weights));
                memset(&pbmp, 0, sizeof(bcm_pbmp_t));
                BCM_PBMP_PORT_ADD(pbmp, phy_port);
                switch(method)
                {
                        case DEV_SWDRVL4_StrictPriorityScheduling :
                                error = bcm_cosq_port_sched_set((int)device_id, pbmp, BCM_COSQ_STRICT, zero_weights, 0);
                                break;
                        case DEV_SWDRVL4_SpWithWrrScheduling :
                        case DEV_SWDRVL4_WeightedRoundRobinScheduling :
                                error = bcm_cosq_port_sched_set((int)device_id, pbmp, BCM_COSQ_WEIGHTED_ROUND_ROBIN, dflt_weights, 0);
                                break;
                        case DEV_SWDRVL4_SpWithDrrScheduling :
                        case DEV_SWDRVL4_DeficitRoundRobinScheduling :
                            for(i = 0; i < SYS_ADPT_MAX_NBR_OF_PRIORITY_QUEUE; i++)
                                dflt_weights[i] = dflt_weights[i] * SYS_DFLT_DRR_GRANULARITY;
                                error = bcm_cosq_port_sched_set((int)device_id, pbmp, BCM_COSQ_DEFICIT_ROUND_ROBIN, dflt_weights, 0);
                                break;
                        default:
                                break;
                }
                if(BCM_E_NONE != error)
                        return FALSE;
        }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetEgressSpSchedulingQueues
 *------------------------------------------------------------------------------
 * FUNCTION: This function identify which queues are SP scheduling
 *           when the scheduling method is SP + WRR
 * INPUT   : unit_id -- physically stacking unit(box) number
 *           port    -- user port in this unitstart_queue_no -- the starting
 *                      queue number for SP scheduling
 *           end_queue_no   -- the ending queue number for SP scheduling
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : The queue numbers not included in start_queue_no and end_queue_no
 *           will run WRR scheduling
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetEgressSpSchedulingQueues(UI32_T unit_id,
                                               UI32_T port,
                                               UI32_T start_queue_no,
                                               UI32_T end_queue_no)
{
    return FALSE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortWrrQueueWeight
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets WRR queue weight
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 *           q_id     -- which queue to set
 *           weight   -- which weight to set
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: failed
 * NOTE    : The weight is a relative value to represent the ratio
 *           among queues
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortWrrQueueWeight(UI32_T unit_id,
                                         UI32_T port,
                                         UI32_T q_id,
                                         UI32_T weight)
{
    int     error;
    uint32  curr_val;
    UI32_T  device_id, my_module_id, phy_port;
    bcm_pbmp_t pbmp;
    #ifdef EIF8X10G
    UI32_T  port_num[DEV_SWDRVL4_MAX_DEVICE_CONNECTED_TO_HERCULES];
    #endif

    DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &my_module_id, &device_id, &phy_port);

        if ((weight & ~0xf) != 0)
            return FALSE;

    if (SOC_IS_TUCANA(device_id))
    {
        assert((q_id>=0) && (q_id <8));

        if ((error = READ_WFQMINBWCOSr(device_id, phy_port, q_id ,&curr_val)) != 0)
            return FALSE;

        soc_reg_field_set(device_id, WFQMINBWCOSr, &curr_val, MIN_BWf, weight);

        error = WRITE_WFQMINBWCOSr(device_id, phy_port, q_id, curr_val);

    }

    if (SOC_IS_DRACO(device_id) || SOC_IS_LYNX(device_id))
    {
        error = READ_WFQWEIGHTSr(device_id, phy_port, &curr_val);

        if (error != BCM_E_NONE)
            return FALSE;

        if (q_id == 0)
            soc_reg_field_set(device_id, WFQWEIGHTSr, &curr_val, COS0WEIGHTf, weight);
        else if (q_id == 1)
            soc_reg_field_set(device_id, WFQWEIGHTSr, &curr_val, COS1WEIGHTf, weight);
        else if (q_id == 2)
            soc_reg_field_set(device_id, WFQWEIGHTSr, &curr_val, COS2WEIGHTf, weight);
        else if (q_id == 3)
            soc_reg_field_set(device_id, WFQWEIGHTSr, &curr_val, COS3WEIGHTf, weight);
        else if (q_id == 4)
            soc_reg_field_set(device_id, WFQWEIGHTSr, &curr_val, COS4WEIGHTf, weight);
        else if (q_id == 5)
            soc_reg_field_set(device_id, WFQWEIGHTSr, &curr_val, COS5WEIGHTf, weight);
        else if (q_id == 6)
            soc_reg_field_set(device_id, WFQWEIGHTSr, &curr_val, COS6WEIGHTf, weight);
        else if (q_id == 7)
            soc_reg_field_set(device_id, WFQWEIGHTSr, &curr_val, COS7WEIGHTf, weight);

        error = WRITE_WFQWEIGHTSr(device_id, phy_port, curr_val);
    }

    if (SOC_IS_HELIX(device_id) || SOC_IS_FIREBOLT(device_id) || SOC_IS_EASYRIDER(device_id))
    {
        error = READ_WRRWEIGHTSr(device_id, phy_port, &curr_val);

        if (error != BCM_E_NONE)
            return FALSE;

        if (q_id == 0)
        soc_reg_field_set(device_id, WRRWEIGHTSr, &curr_val,
                          COS0WEIGHTf, weight);
        if (q_id == 1)
        soc_reg_field_set(device_id, WRRWEIGHTSr, &curr_val,
                          COS1WEIGHTf, weight);
        if (q_id == 2)
        soc_reg_field_set(device_id, WRRWEIGHTSr, &curr_val,
                          COS2WEIGHTf, weight);
        if (q_id == 3)
        soc_reg_field_set(device_id, WRRWEIGHTSr, &curr_val,
                          COS3WEIGHTf, weight);
        if (q_id == 4)
        soc_reg_field_set(device_id, WRRWEIGHTSr, &curr_val,
                          COS4WEIGHTf, weight);
        if (q_id == 5)
        soc_reg_field_set(device_id, WRRWEIGHTSr, &curr_val,
                          COS5WEIGHTf, weight);
        if (q_id == 6)
        soc_reg_field_set(device_id, WRRWEIGHTSr, &curr_val,
                          COS6WEIGHTf, weight);
        if (q_id == 7)
        soc_reg_field_set(device_id, WRRWEIGHTSr, &curr_val,
                          COS7WEIGHTf, weight);
        error = WRITE_WRRWEIGHTSr(device_id, phy_port, curr_val);
    }

        /* 2008-06-02, Jinfeng.Chen: add to support firebolt2 */
        if(SOC_IS_RAVEN(device_id) || SOC_IS_FIREBOLT2(device_id)) /* 2009/08/04, Eric, for trupicana , it should use this api to check */
        {
                int i;
                int mode;
                int tmp_weight[BCM_COS_COUNT];

                for(i = 0; i < BCM_COS_COUNT; i++)
                        tmp_weight[i] = 1;

                memset(&pbmp, 0, sizeof(bcm_pbmp_t));
                BCM_PBMP_PORT_ADD(pbmp, phy_port);
                error = bcm_cosq_port_sched_get(device_id, pbmp, &mode, tmp_weight, 0);
                if(BCM_E_NONE != error)
                {
                        return FALSE;
                }
                switch(mode)
                {
                        case BCM_COSQ_DEFICIT_ROUND_ROBIN:
                                tmp_weight[q_id] = weight * SYS_DFLT_DRR_GRANULARITY;
                                break;
                        default:
                                tmp_weight[q_id] = weight;
                                break;
                }

                error = bcm_cosq_port_sched_set(device_id, pbmp, mode, tmp_weight, 0);
                }

    if (BCM_E_NONE != error)
        return FALSE;

    swdrvl4_wrr_queue[device_id][phy_port].queue_id[q_id] = weight;

#ifdef EIF8X10G
    /*set queue weight to 5670*/
    STKTPLG_POM_GetHiGiPortNum(&port_num);

    WRITE_MMU_EGS_WGTCOSr(DEV_SWDRVL4_HERCULES_DEVICE_ID, port_num[my_module_id], q_id, (100*weight));
#endif


    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortWrrQueueWeight
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets WRR queue weight
 * INPUT   : unit -- physically stacking unit(box) number
 *           port -- user port in this unit
 *           weight   -- whole weight to set
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: failed
 * NOTE    : Each weight is a relative value to represent the ratio
 *           among queues
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetWrrQueueWeightAll(UI32_T unit_id, UI32_T port, UI32_T *weight)
{
#if 0
    int error;
    bcm_pbmp_t pbmp;
#endif
    UI32_T  device_id, my_module_id, phy_port;

    DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &my_module_id, &device_id, &phy_port);

#if 0
    if (SOC_IS_RAPTOR(device_id))
    {
        memset(&pbmp, 0, sizeof(bcm_pbmp_t));
        BCM_PBMP_PORT_ADD(pbmp, phy_port);
        error = bcm_cosq_port_sched_set(device_id, pbmp, BCM_COSQ_WEIGHTED_ROUND_ROBIN, weight, 0);
    }

    if (error != BCM_E_NONE)
        return FALSE;
#endif

    return TRUE;
}
/*=====================================================================
 * Exported functions for Traffic Segmatation
 *=====================================================================
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnableTrafficSegmatation
 *------------------------------------------------------------------------------
 * FUNCTION: Enable Traffic Segematation functions
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    :
 *
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_EnableTrafficSegmatation()
{
    if (DEV_SWDRVL4_TrafficSegmatationStatus == FALSE)
    {
        DEV_SWDRVL4_TrafficSegmatationStatus = TRUE;
    }
    return TRUE;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DisableTrafficSegematation
 *------------------------------------------------------------------------------
 * FUNCTION: Disable Traffic Segematation functions
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    :
 *
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DisableTrafficSegmatation()
{
    if (DEV_SWDRVL4_TrafficSegmatationStatus == TRUE)
    {
        /* clear all EGRESS_MASK entry */
        if (FALSE == DEV_SWDRVL4_ResetTrafficSegmatation())
        {
            return FALSE;
        }
        DEV_SWDRVL4_TrafficSegmatationStatus = FALSE;
    }
    return TRUE;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetTrafficSegmatation
 *------------------------------------------------------------------------------
 * FUNCTION: Set the traffic segematation membership
 * INPUT   : uplink_uport_list     - uplink port list member set
 *           uplink_uport_list_cnt - uplink port list member count
 *           downlink_port_list    - downlink port list member set
 *           downlink_uport_list_cnt - downlink port list member count
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : (1) MSB is port 1
 *           (2) For downlink member, it can send traffic only to uplink member,
 *               for uplink member, it can send traffic to any port, for other
 *               ports, thay have no limitation on this. The isolation is exist
 *               only :
 *              <1) from downlink member to downlink member
 *              <2> from downlink member to non-uplink ports
 *           (3) For cross chip, traffic from the other side will look egress mask
 *               of end side. Thus, for each port in downlink list, we have to set
 *               different mask for the same index on different chip
 *           (4) Currently, not support trunk
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetTrafficSegmatation(  SYS_TYPE_Uport_T    *uplink_uport_list,
                                            UI32_T               uplink_uport_list_cnt,
                                            SYS_TYPE_Uport_T    *downlink_uport_list,
                                            UI32_T               downlink_uport_list_cnt
                                         )
{
    UI32_T     uplink_port_cnt, downlink_port_cnt, uplink_module_id, uplink_device_id, uplink_phy_port, downlink_module_id, downlink_device_id, downlink_phy_port;
    pbmp_t     pbmp[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT]; /* soc_ndev --> SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT */
    pbmp_t     temp_pbmp[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];
    UI32_T     my_unit_id, device_id;
#if 0
    UI32_T     hg_port, temp_phy_port;
    int        error;
#endif

    if (DEV_SWDRVL4_TrafficSegmatationStatus == FALSE)
    {
        return FALSE;
    }

    if (FALSE == STKTPLG_POM_GetMyUnitID(&my_unit_id))
    {
        return FALSE;
    }

    /* Step 1: clear last setting */
    if (FALSE == DEV_SWDRVL4_ResetTrafficSegmatation())
    {
        return FALSE;
    }

    /* Step 2: Clear the port bit map*/
    for (device_id=0; device_id<soc_ndev; device_id++)
    {
#if 0
        if (SOC_IS_TUCANA(device_id) || SOC_IS_DRACO(device_id) || SOC_IS_LYNX(device_id) ||
            SOC_IS_HELIX(device_id)  || SOC_IS_FIREBOLT(device_id) || SOC_IS_EASYRIDER(device_id) ||
            SOC_IS_RAPTOR(device_id))    /*add by wx for support bcm566x family chipset*/
        {
            BCM_PBMP_CLEAR(pbmp[device_id]);
        }
#endif
    }

    /* Step 3: Add HG port and CPU port to pbmp. */
    for (device_id=0; device_id<soc_ndev; device_id++)
    {
#if 0
        if ( SOC_IS_TUCANA(device_id) || SOC_IS_DRACO(device_id)    || SOC_IS_LYNX(device_id) ||
             SOC_IS_HELIX(device_id)  || SOC_IS_FIREBOLT(device_id) || SOC_IS_EASYRIDER(device_id) ||
             SOC_IS_RAPTOR(device_id))
        {
            PBMP_ALL_ITER(device_id, temp_phy_port)
            {
                if ((IS_HG_PORT(device_id, temp_phy_port)) || (IS_CPU_PORT(device_id, temp_phy_port)))
                {
                    BCM_PBMP_PORT_ADD(pbmp[device_id], temp_phy_port);
                }
            }
        }
#endif
    }

    /*Step 4: Prepare the UpLink port Bit Map. we save the portbitmap into pbmp[].*/
    for (uplink_port_cnt = 0; uplink_port_cnt < uplink_uport_list_cnt; uplink_port_cnt++)
    {
        /* only the devices within my_unit_id should added to the pbmp*/
        if (uplink_uport_list[uplink_port_cnt].unit == my_unit_id)
        {
            if (DEV_SWDRV_Logical2PhyDevicePortID(uplink_uport_list[uplink_port_cnt].unit, uplink_uport_list[uplink_port_cnt].port, &uplink_module_id, &uplink_device_id, &uplink_phy_port) == FALSE)
                return FALSE;

            SOC_PBMP_PORT_ADD(pbmp[uplink_device_id], uplink_phy_port);
        }
    }

    memcpy(temp_pbmp, pbmp, sizeof(pbmp_t)*SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT);
    /*Step 5: Set to the chip*/
    for (downlink_port_cnt = 0; downlink_port_cnt < downlink_uport_list_cnt; downlink_port_cnt++)
    {
        memcpy(pbmp, temp_pbmp, sizeof(pbmp_t)*SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT);
        /* only the devices within my_unit_id should added to the pbmp*/
        if (downlink_uport_list[downlink_port_cnt].unit == my_unit_id)
        {
            if (DEV_SWDRV_Logical2PhyDevicePortID(downlink_uport_list[downlink_port_cnt].unit, downlink_uport_list[downlink_port_cnt].port, &downlink_module_id, &downlink_device_id, &downlink_phy_port) == FALSE)
                return FALSE;

            for (device_id=0; device_id<soc_ndev; device_id++)
            {
#if 0
                if ( SOC_IS_TUCANA(device_id) || SOC_IS_DRACO(device_id)    || SOC_IS_LYNX(device_id) ||
                     SOC_IS_HELIX(device_id)  || SOC_IS_FIREBOLT(device_id) || SOC_IS_EASYRIDER(device_id) ||
                     SOC_IS_RAPTOR(device_id))
                {
                    /* If this module have downlink port, need to add Higi port into egress*/
                    if (SOC_IS_DRACO(device_id) || SOC_IS_LYNX(device_id))
                    {
                        if ((my_unit_id == downlink_uport_list[downlink_port_cnt].unit) && (downlink_device_id == device_id))
                        {
                            hg_port=IPIC_PORT(device_id);
                            BCM_PBMP_PORT_ADD(pbmp[device_id], hg_port);
                        }
                    }
/* modify Macro defined (ES4626H) || defined (ECN430_FB2),michael.wang 2008-6-25 */
#if (SYS_CPNT_TWOCHIPSTACKINGINONEBOARD == TRUE)
                    if (soc_ndev == 2) /* 50 ports */
                    {
                        if ( SOC_IS_HELIX(device_id)  || SOC_IS_FIREBOLT(device_id) || SOC_IS_EASYRIDER(device_id))
                                    {
                            if ( device_id == 0 )
                            {
                                if ( (downlink_phy_port>=0) && (downlink_phy_port<12) )
                                    BCM_PBMP_PORT_REMOVE(pbmp[device_id], DEV_SWDRV_XGSIII_50PORT_BOARD_INTER_HG_PORT2);
                                else if ( (downlink_phy_port>=12) && (downlink_phy_port<24) )
                                    BCM_PBMP_PORT_REMOVE(pbmp[device_id], DEV_SWDRV_XGSIII_50PORT_BOARD_INTER_HG_PORT1);
                            }
                            else if ( device_id == 1 )
                            {
                                if ( (downlink_phy_port>=0) && (downlink_phy_port<12) )
                                    BCM_PBMP_PORT_REMOVE(pbmp[device_id], DEV_SWDRV_XGSIII_50PORT_BOARD_INTER_HG_PORT1);
                                else if ( (downlink_phy_port>=12) && (downlink_phy_port<24) )
                                    BCM_PBMP_PORT_REMOVE(pbmp[device_id], DEV_SWDRV_XGSIII_50PORT_BOARD_INTER_HG_PORT2);
                            }
                        }
                    }
#endif
                    error = bcm_port_egress_set(device_id, downlink_phy_port, downlink_module_id, pbmp[device_id]);
                    if (error != BCM_E_NONE)
                    {
                        DEV_SWDRVL4_CHIP_ERROR_MSG(downlink_phy_port, error);
                        return FALSE;
                    }
                }
#endif
            }
        }
    }
    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetTrafficSegmatationByPortlist
 *------------------------------------------------------------------------------
 * FUNCTION: Set the traffic segematation membership
 * INPUT   : uplink_uport_list     - uplink port list member set
 *           uplink_uport_list_cnt - uplink port list member count
 *               downlink_port_list    - downlink port list member set
 *           downlink_uport_list_cnt - downlink port list member count
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : (1) MSB is port 1
 *           (2) For downlink member, it can send traffic only to uplink member,
 *               for uplink member, it can send traffic to any port, for other
 *               ports, thay have no limitation on this. The isolation is exist
 *               only :
 *              <1) from downlink member to downlink member
 *              <2> from downlink member to non-uplink ports
 *           (3) For cross chip, traffic from the other side will look egress mask
 *               of end side. Thus, for each port in downlink list, we have to set
 *               different mask for the same index on different chip
 *           (4) Currently, not support trunk
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetTrafficSegmatationByPortlist(UI8_T *uplink_port_list,
                                                   UI8_T *downlink_port_list)
{
    UI32_T  uplink_uport_list_cnt = 0;
    UI32_T  downlink_uport_list_cnt = 0;

    UI32_T  my_unit_id, ifindex, device_id;
    UI32_T  count, main_board_port_num;
    UI32_T  uplink_module_id, uplink_device_id, uplink_phy_port;
    UI32_T  downlink_module_id, downlink_device_id, downlink_phy_port;
    pbmp_t  pbmp[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT]; /* soc_ndev --> SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT */
    pbmp_t     temp_pbmp[SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT];
#if 1
    int     error;
    UI32_T  temp_phy_port;;
#endif

    /* if pvlan is disable, we should not set into chip. Just need to update om
     */
    if (DEV_SWDRVL4_TrafficSegmatationStatus == FALSE)
    {
        return TRUE;
    }


    if (FALSE == STKTPLG_POM_GetMyUnitID(&my_unit_id))
    {
        return FALSE;
    }

    /* Step 1: clear last setting */
    if (FALSE == DEV_SWDRVL4_ResetTrafficSegmatation())
    {
        return FALSE;
    }
#if (SYS_DFLT_TRAFFIC_SEG_METHOD != SYS_DFLT_TRAFFIC_SEG_METHOD_PORT_PRIVATE_MODE)
    /* Transfer uplink port list bitmap to user port list */
    ifindex = 0;
    while (DEV_SWDRVL4_GetNextIndexFromPortList(&ifindex, uplink_port_list))
    {
        uplink_uport_list[uplink_uport_list_cnt].unit = DEV_SWDRVL4_IFINDEX_TO_UNIT(ifindex);
        uplink_uport_list[uplink_uport_list_cnt].port = DEV_SWDRVL4_IFINDEX_TO_PORT(ifindex);
        uplink_uport_list_cnt++;
    } /* End of while */
#endif

    /* Transfer downlink port list bitmap to user port list */
    ifindex = 0;
    while (DEV_SWDRVL4_GetNextIndexFromPortList(&ifindex, downlink_port_list))
    {
        downlink_uport_list[downlink_uport_list_cnt].unit = DEV_SWDRVL4_IFINDEX_TO_UNIT(ifindex);
        downlink_uport_list[downlink_uport_list_cnt].port = DEV_SWDRVL4_IFINDEX_TO_PORT(ifindex);
        downlink_uport_list_cnt++;
    } /* End of while */

    /* Step 2: Clear the port bit map*/
    for (device_id=0; device_id<soc_ndev; device_id++)
    {
#if 1
        if(SOC_IS_TUCANA(device_id) || SOC_IS_DRACO(device_id) || SOC_IS_LYNX(device_id) || SOC_IS_ROBO(device_id)
           || SOC_IS_FIREBOLT(device_id) || SOC_IS_RAVEN(device_id))    /*add by wx for support bcm566x family chipset, raven is used for trupicana*/
        {
            SOC_PBMP_CLEAR(pbmp[device_id]);
        }
#endif
    }
#if (SYS_DFLT_TRAFFIC_SEG_METHOD != SYS_DFLT_TRAFFIC_SEG_METHOD_PORT_PRIVATE_MODE)
    /*Step 3: Prepare the UpLink port Bit Map. we save the portbitmap into pbmp[].*/
    for (count = 0; count < uplink_uport_list_cnt; count++)
    {
        /* only the devices within my_unit_id should added to the pbmp*/
        if (uplink_uport_list[count].unit == my_unit_id)
        {
            if (STKTPLG_POM_IsOptionModule())
            {
                main_board_port_num = 0;
                STKTPLG_POM_GetMainBoardPortNum(&main_board_port_num);

                if (uplink_uport_list[count].port <= main_board_port_num)
                {
                    continue;
                }
            }
            else
            {
                if (STKTPLG_POM_IsModulePort(uplink_uport_list[count].unit,
                                            uplink_uport_list[count].port))
                {
                    continue;
                }
            }

            if (DEV_SWDRV_Logical2PhyDevicePortID(uplink_uport_list[count].unit, uplink_uport_list[count].port, &uplink_module_id, &uplink_device_id, &uplink_phy_port) == FALSE)
            {
                return FALSE;
            }

            SOC_PBMP_PORT_ADD(pbmp[uplink_device_id], uplink_phy_port);
        }
    }

    memcpy(temp_pbmp, pbmp, sizeof(pbmp_t)*SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT);
    /*Step 4: Set to the chip*/
    for (count = 0; count < downlink_uport_list_cnt; count++)
    {
        memcpy(pbmp, temp_pbmp, sizeof(pbmp_t)*SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT);

        if (DEV_SWDRV_Logical2PhyDevicePortID(downlink_uport_list[count].unit, downlink_uport_list[count].port, &downlink_module_id, &downlink_device_id, &downlink_phy_port) == FALSE)
        {
            return FALSE;
        }

        for (device_id=0; device_id<soc_ndev; device_id++)
        {
#if 1
            if(SOC_IS_TUCANA(device_id) || SOC_IS_DRACO(device_id) || SOC_IS_LYNX(device_id)
               || SOC_IS_FIREBOLT(device_id) || SOC_IS_RAVEN(device_id))    /*add by wx for support bcm566x family chipset, raven is used for trupicana*/
            {
#if 0

                hg_port   = IPIC_PORT(device_id);
                cmic_port = CMIC_PORT(device_id);

                SOC_PBMP_PORT_ADD(pbmp[device_id], hg_port);
                SOC_PBMP_PORT_ADD(pbmp[device_id], cmic_port);

#endif

                PBMP_ALL_ITER(device_id, temp_phy_port)
                {
                    if ((IS_HG_PORT(device_id, temp_phy_port)) || (IS_CPU_PORT(device_id, temp_phy_port)))
                    {
                        BCM_PBMP_PORT_ADD(pbmp[device_id], temp_phy_port);
                    }
                }

                error = bcm_port_egress_set(device_id, downlink_phy_port, downlink_module_id, pbmp[device_id]);

                if (error != BCM_E_NONE)
                {
                    DEV_SWDRVL4_CHIP_ERROR_MSG(downlink_phy_port, error);
                    return FALSE;
                }
            }
#endif
        }
    }
#else

#ifdef BCM_ROBO_SUPPORT
    /*Step 3: Prepare the downlink port Bit Map. we save the portbitmap into pbmp[].*/

    for(device_id = 0; device_id<soc_ndev; device_id++)
        for (count = 0; count < downlink_uport_list_cnt; count++)
        {
            if (DEV_SWDRV_Logical2PhyDevicePortID(downlink_uport_list[count].unit, downlink_uport_list[count].port, &downlink_module_id, &downlink_device_id, &downlink_phy_port) == FALSE)
            {
                return FALSE;
            }
            SOC_PBMP_PORT_ADD(pbmp[device_id], downlink_phy_port);
        }

    /* Step 4: Set to the chip */
    error = soc_reg32_write(device_id, PROTECTED_PORT_SELECTf, pbmp[device_id]);
    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(0, error);
        return FALSE;
    }
#endif
#endif
    return TRUE;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_ResetTrafficSegmatation
 *------------------------------------------------------------------------------
 * FUNCTION: Reset the Traffic Segematation (Delete the membership in the H/W)
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : Clear all the private vlan irules
 *
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_ResetTrafficSegmatation()
{
    UI32_T module_id, device_id, phy_port;
    UI32_T unit, port, trunk_id;
    BOOL_T ret = TRUE;

    int error;

    /* update chip
     */
    for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; unit++)
    {
        for (port = 1; port <= SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT_ON_BOARD; port++)
        {
            if (!DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port))
            {
                continue;
            }

            for (device_id = 0; device_id < soc_ndev; device_id++)
            {
                int my_modid, tmp_modid, tmp_port;
                pbmp_t tmp_pbmp;

                tmp_modid = module_id;
                tmp_port = phy_port;

#if (SYS_ADPT_BCM_DUAL_MODID_MODE == TRUE)
                if (BCM_E_NONE == bcm_stk_my_modid_get(device_id, &my_modid))
                {
                    if (my_modid != module_id)
                    {
                        tmp_modid += (phy_port / 32);
                        tmp_port = (phy_port % 32);
                    }
                }
#endif

                error = bcm_port_egress_get(device_id, tmp_port, tmp_modid, &tmp_pbmp);

                BCM_PBMP_OR(tmp_pbmp, PBMP_E_ALL(device_id));

                error = bcm_port_egress_set(device_id, tmp_port, tmp_modid, tmp_pbmp);
                if (error != BCM_E_NONE)
                {
                    DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
                    ret = FALSE;
                }
            } /* end of for (device_id) */
        } /* end of for (port) */
    } /* end of for (unit) */

    for (trunk_id = 1; trunk_id <= SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM; trunk_id++)
    {
        /* update chip with localPortsMembersPtr
         */
        for (device_id = 0; device_id < soc_ndev; device_id++)
        {
            pbmp_t tmp_pbmp;

            error = bcm_trunk_egress_get(device_id, trunk_id-1, &tmp_pbmp);

            BCM_PBMP_OR(tmp_pbmp, PBMP_E_ALL(device_id));

            error = bcm_trunk_egress_set(device_id, trunk_id-1, tmp_pbmp);
            if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
            {
                DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
                ret = FALSE;
            }
        } /* end of for (device_id) */
    } /* end of for (trunk_id) */

    return ret;
}

#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE)
#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION)
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_UpdatePrivateVlan
 *------------------------------------------------------------------------------
 * FUNCTION: This function will update chip for private vlan
 * INPUT   : uplink_port_list   -- uplink port list or NULL
 *           downlink_port_list -- downlink port list or NULL
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_UpdatePrivateVlan(UI8_T  *uplink_port_list,
                                     UI8_T  *downlink_port_list)
{
    UI32_T module_id, device_id, phy_port;
    UI32_T unit, port, trunk_id;
    UI32_T tmp_session_id;
    UI32_T byte_index;
    UI8_T bit_value;
    BOOL_T ret = TRUE;

    int error;
    pbmp_t *egr_pbmp = NULL;

    /* update chip
     */
    for (unit = 1; unit <= SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK; unit++)
    {
        for (port = 1; port <= SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT_ON_BOARD; port++)
        {
            byte_index = (unit - 1) * SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST + (port - 1) / 8;
            bit_value = BIT_VALUE(7 - ((port - 1) % 8));

            /* update for specified port list and all uplink ports
             */
            if ((uplink_port_list &&
                    (uplink_port_list[byte_index] & bit_value) != 0) ||
                (downlink_port_list &&
                    (downlink_port_list[byte_index] & bit_value) != 0) ||
                (dev_swdrvl4_traffic_seg.port_session_id[unit-1][port-1] != 0))
            {
                if (!DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port))
                {
                    continue;
                }

                /* update chip with localPortsMembersPtr
                 */
                tmp_session_id = dev_swdrvl4_traffic_seg.port_session_id[unit-1][port-1];

                for (device_id = 0; device_id < soc_ndev; device_id++)
                {
                    int my_modid, tmp_modid, tmp_port;
                    pbmp_t tmp_pbmp;

                    if (tmp_session_id == 0)
                    {
                        egr_pbmp = &dev_swdrvl4_traffic_seg.normal_egr_pbmp[device_id];
                    }
                    else if ((dev_swdrvl4_traffic_seg.uplink_list[byte_index] & bit_value) != 0)
                    {
                        egr_pbmp = &dev_swdrvl4_traffic_seg.session_info[tmp_session_id-1].uplink_egr_pbmp[device_id];
                    }
                    else
                    {
                        egr_pbmp = &dev_swdrvl4_traffic_seg.session_info[tmp_session_id-1].downlink_egr_pbmp[device_id];
                    }

                    tmp_modid = module_id;
                    tmp_port = phy_port;

#if (SYS_ADPT_BCM_DUAL_MODID_MODE == TRUE)
                    if (BCM_E_NONE == bcm_stk_my_modid_get(device_id, &my_modid))
                    {
                        if (my_modid != module_id)
                        {
                            tmp_modid += (phy_port / 32);
                            tmp_port = (phy_port % 32);
                        }
                    }
#endif

                    error = bcm_port_egress_get(device_id, tmp_port, tmp_modid, &tmp_pbmp);

                    BCM_PBMP_REMOVE(tmp_pbmp, PBMP_E_ALL(device_id));
                    BCM_PBMP_OR(tmp_pbmp, *egr_pbmp);

                    error = bcm_port_egress_set(device_id, tmp_port, tmp_modid, tmp_pbmp);
                    if (error != BCM_E_NONE)
                    {
                        DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
                        ret = FALSE;
                    }
                } /* end of for (device_id) */
            }
        } /* end of for (port) */
    } /* end of for (unit) */

    for (trunk_id = 1; trunk_id <= SYS_ADPT_MAX_NBR_OF_TRUNK_PER_SYSTEM; trunk_id++)
    {
        /* update chip with localPortsMembersPtr
         */
        tmp_session_id = dev_swdrvl4_traffic_seg.trunk_session_id[trunk_id-1];

        for (device_id = 0; device_id < soc_ndev; device_id++)
        {
            int my_modid, tmp_modid, tmp_port;
            pbmp_t tmp_pbmp;

            if (tmp_session_id == 0)
            {
                egr_pbmp = &dev_swdrvl4_traffic_seg.normal_egr_pbmp[device_id];
            }
            else if (dev_swdrvl4_traffic_seg.trunk_is_uplink[trunk_id-1])
            {
                egr_pbmp = &dev_swdrvl4_traffic_seg.session_info[tmp_session_id-1].uplink_egr_pbmp[device_id];
            }
            else
            {
                egr_pbmp = &dev_swdrvl4_traffic_seg.session_info[tmp_session_id-1].downlink_egr_pbmp[device_id];
            }

            error = bcm_trunk_egress_get(device_id, trunk_id-1, &tmp_pbmp);

            BCM_PBMP_REMOVE(tmp_pbmp, PBMP_E_ALL(device_id));
            BCM_PBMP_OR(tmp_pbmp, *egr_pbmp);

            error = bcm_trunk_egress_set(device_id, trunk_id-1, tmp_pbmp);
            if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
            {
                DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
                ret = FALSE;
            }
        } /* end of for (device_id) */
    } /* end of for (trunk_id) */

    return ret;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_PrivateVlan_Init
 *------------------------------------------------------------------------------
 * FUNCTION: This function will init private vlan database and chip
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_PrivateVlan_Init(void)
{
    UI32_T device_id, phy_port;
    UI32_T session_id;
    UI8_T port_list[SYS_ADPT_MAX_NBR_OF_UNIT_PER_STACK * SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST];

    /* init database
     */
    pbmp_t pbmp;

    memset(&dev_swdrvl4_traffic_seg, 0, sizeof(dev_swdrvl4_traffic_seg));

    for (device_id = 0; device_id < soc_ndev; device_id++)
    {
        BCM_PBMP_CLEAR(pbmp);

        PBMP_E_ITER(device_id, phy_port)
        {
            /*
            if(DEV_SWDRV_IS_CRAFT_PORT(device_id, phy_port)==TRUE)
                continue;
             */

            BCM_PBMP_PORT_ADD(pbmp, phy_port);
        } /* end of for (phy_port) */

        for (session_id = 1; session_id <= SYS_ADPT_PORT_TRAFFIC_SEGMENTATION_MAX_NBR_OF_SESSIONS; session_id++)
        {
            BCM_PBMP_ASSIGN(dev_swdrvl4_traffic_seg.normal_egr_pbmp[device_id], pbmp);
            BCM_PBMP_ASSIGN(dev_swdrvl4_traffic_seg.session_info[session_id-1].uplink_egr_pbmp[device_id], pbmp);
        }
    } /* end of for (device_id) */

    /* reset chip
     */
    memset(port_list, 0xff, sizeof(port_list));

    if (!DEV_SWDRVL4_UpdatePrivateVlan(NULL, port_list))
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_UpdatePrivateVlan failed");
        return FALSE;
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPrivateVlanPortlistBySessionId
 *------------------------------------------------------------------------------
 * FUNCTION: This function will set the private vlan by session group
 * INPUT   : session_id         -- session id to pvlan group
 *           uplink_port_list   -- uplink port list
 *           downlink_port_list -- downlink port list
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPrivateVlanPortlistBySessionId(UI32_T session_id,
                                                     UI8_T  *uplink_port_list,
                                                     UI8_T  *downlink_port_list)
{
    UI32_T module_id, device_id, phy_port;
    UI32_T my_unit_id;
    UI32_T unit, port;
    UI32_T tmp_session_id;
    UI32_T byte_index;
    UI8_T bit_value;

    /* update database
     */
    if (FALSE == STKTPLG_POM_GetMyUnitID(&my_unit_id))
    {
        return FALSE;
    }

    unit = my_unit_id;

    for (port = 1; port <= SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT_ON_BOARD; port++)
    {
        if (!DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port))
        {
            continue;
        }

        byte_index = (unit - 1) * SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST + (port - 1) / 8;
        bit_value = BIT_VALUE(7 - ((port - 1) % 8));

        if (dev_swdrvl4_traffic_seg.port_session_id[unit-1][port-1] == 0)
        {
            if ((uplink_port_list[byte_index] & bit_value) != 0)
            {
                /* normal -> uplink
                 *
                 * downlink of the same session: block -> forward
                 * downlink of other sessions: block -> block
                 *
                 * if up-to-up is block
                 *      uplink of the same session: forward -> forward
                 *      uplink of other sessions: forward -> block
                 * if up-to-up is forward
                 *      uplink of the same session: forward -> forward
                 *      uplink of other sessions: forward -> forward
                 *
                 * normal ports: forward -> forward
                 */
                BCM_PBMP_PORT_ADD(
                    dev_swdrvl4_traffic_seg.session_info[session_id-1].downlink_egr_pbmp[device_id],
                    phy_port);

                if (dev_swdrvl4_traffic_seg.is_blocking_uplink_group)
                {
                    for (tmp_session_id = 1; tmp_session_id <= SYS_ADPT_PORT_TRAFFIC_SEGMENTATION_MAX_NBR_OF_SESSIONS; tmp_session_id++)
                    {
                        if (tmp_session_id == session_id)
                            continue;

                        BCM_PBMP_PORT_REMOVE(
                            dev_swdrvl4_traffic_seg.session_info[tmp_session_id-1].uplink_egr_pbmp[device_id],
                            phy_port);
                    }
                }

                dev_swdrvl4_traffic_seg.port_session_id[unit-1][port-1] = session_id;
                dev_swdrvl4_traffic_seg.uplink_list[byte_index] |= bit_value;
            } /* end of if (uplink_port_list)*/

            if ((downlink_port_list[byte_index] & bit_value) != 0)
            {
                /* normal -> downlink
                 *
                 * downlink of the same session: block -> block
                 * downlink of other sessions: block -> block
                 *
                 * if up-to-up is block
                 *      uplink of the same session: forward -> forward
                 *      uplink of other sessions: forward -> block
                 * if up-to-up is forward
                 *      uplink of the same session: forward -> forward
                 *      uplink of other sessions: forward -> block
                 *
                 * normal ports: forward -> forward
                 */
                for (tmp_session_id = 1; tmp_session_id <= SYS_ADPT_PORT_TRAFFIC_SEGMENTATION_MAX_NBR_OF_SESSIONS; tmp_session_id++)
                {
                    if (tmp_session_id == session_id)
                        continue;

                    BCM_PBMP_PORT_REMOVE(
                        dev_swdrvl4_traffic_seg.session_info[tmp_session_id-1].uplink_egr_pbmp[device_id],
                        phy_port);
                }

                dev_swdrvl4_traffic_seg.port_session_id[unit-1][port-1] = session_id;
                dev_swdrvl4_traffic_seg.uplink_list[byte_index] &= ~bit_value;
            } /* end of if (downlink_port_list)*/
        }
    } /* end of for (port) */

    /* update chip
     */
    if (!DEV_SWDRVL4_UpdatePrivateVlan(uplink_port_list, downlink_port_list))
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_UpdatePrivateVlan failed");
        return FALSE;
    }

    return TRUE;
}/* End of DEV_SWDRVL4_SetPrivateVlanPortlistBySessionId() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_DeletePrivateVlanPortlistBySessionId
 *------------------------------------------------------------------------------
 * FUNCTION: This function will delete the private vlan by session group
 * INPUT   : session_id         -- session id to pvlan group
 *           uplink_port_list   -- uplink port list
 *           downlink_port_list -- downlink port list
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : session_id is 0, means remove pvlan port throw internal session_id
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_DeletePrivateVlanPortlistBySessionId(UI32_T session_id,
                                                     UI8_T  *uplink_port_list,
                                                     UI8_T  *downlink_port_list)
{
    UI32_T module_id, device_id, phy_port;
    UI32_T my_unit_id;
    UI32_T unit, port;
    UI32_T tmp_session_id;
    UI32_T byte_index;
    UI8_T bit_value;

    /* update database
     */
    if (FALSE == STKTPLG_POM_GetMyUnitID(&my_unit_id))
    {
        return FALSE;
    }

    unit = my_unit_id;

    for (port = 1; port <= SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT_ON_BOARD; port++)
    {
        if (!DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port))
        {
            continue;
        }

        byte_index = (unit - 1) * SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST + (port - 1) / 8;
        bit_value = BIT_VALUE(7 - ((port - 1) % 8));

        if (dev_swdrvl4_traffic_seg.port_session_id[unit-1][port-1] != 0)
        {
            if ((uplink_port_list[byte_index] & bit_value) != 0)
            {
                /* uplink -> normal
                 *
                 * downlink of the same session: forward -> block
                 * downlink of other sessions: block -> block
                 *
                 * if up-to-up is block
                 *      uplink of the same session: forward -> forward
                 *      uplink of other sessions: block -> forward
                 * if up-to-up is forward
                 *      uplink of the same session: forward -> forward
                 *      uplink of other sessions: forward -> forward
                 *
                 * normal ports: forward -> forward
                 */
                BCM_PBMP_PORT_REMOVE(
                    dev_swdrvl4_traffic_seg.session_info[session_id-1].downlink_egr_pbmp[device_id],
                    phy_port);

                if (dev_swdrvl4_traffic_seg.is_blocking_uplink_group)
                {
                    for (tmp_session_id = 1; tmp_session_id <= SYS_ADPT_PORT_TRAFFIC_SEGMENTATION_MAX_NBR_OF_SESSIONS; tmp_session_id++)
                    {
                        if (tmp_session_id == session_id)
                            continue;

                        BCM_PBMP_PORT_ADD(
                            dev_swdrvl4_traffic_seg.session_info[tmp_session_id-1].uplink_egr_pbmp[device_id],
                            phy_port);
                    }
                }

                dev_swdrvl4_traffic_seg.port_session_id[unit-1][port-1] = 0;
            } /* end of if (uplink_port_list)*/

            if ((downlink_port_list[byte_index] & bit_value) != 0)
            {
                /* downlink -> normal
                 *
                 * downlink of the same session: block -> block
                 * downlink of other sessions: block -> block
                 *
                 * if up-to-up is block
                 *      uplink of the same session: forward -> forward
                 *      uplink of other sessions: block -> forward
                 * if up-to-up is forward
                 *      uplink of the same session: forward -> forward
                 *      uplink of other sessions: block -> forward
                 *
                 * normal ports: forward -> forward
                 */
                for (tmp_session_id = 1; tmp_session_id <= SYS_ADPT_PORT_TRAFFIC_SEGMENTATION_MAX_NBR_OF_SESSIONS; tmp_session_id++)
                {
                    if (tmp_session_id == session_id)
                        continue;

                    BCM_PBMP_PORT_ADD(
                        dev_swdrvl4_traffic_seg.session_info[tmp_session_id-1].uplink_egr_pbmp[device_id],
                        phy_port);
                }

                dev_swdrvl4_traffic_seg.port_session_id[unit-1][port-1] = 0;
            } /* end of if (downlink_port_list)*/
        }
    } /* end of for (port) */

    /* update chip
     */
    if (!DEV_SWDRVL4_UpdatePrivateVlan(uplink_port_list, downlink_port_list))
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_UpdatePrivateVlan failed");
        return FALSE;
    }

    return TRUE;
}/* End of DEV_SWDRVL4_DeletePrivateVlanPortlistBySessionId() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPrivateVlanUplinkPortMode
 *------------------------------------------------------------------------------
 * FUNCTION: This function will set uplink port mode to private vlan,
 *           means uplink's traffic are blocking each groups
 * INPUT   : Mode  TRUE is blocking, FALSE is forwarding
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPrivateVlanUplinkPortMode(BOOL_T mode)
{
    UI32_T module_id, device_id, phy_port;
    UI32_T my_unit_id;
    UI32_T unit, port;
    UI32_T session_id, tmp_session_id;
    UI32_T byte_index;
    UI8_T bit_value;

    if (mode == dev_swdrvl4_traffic_seg.is_blocking_uplink_group)
    {
        return TRUE;
    }

    /* update database
     */
    if (FALSE == STKTPLG_POM_GetMyUnitID(&my_unit_id))
    {
        return FALSE;
    }

    unit = my_unit_id;

    for (port = 1; port <= SYS_ADPT_MAX_NBR_OF_PORT_PER_UNIT_ON_BOARD; port++)
    {
        if (!DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port))
        {
            continue;
        }

        byte_index = (unit - 1) * SYS_ADPT_NBR_OF_BYTE_FOR_1BIT_UPORT_LIST + (port - 1) / 8;
        bit_value = BIT_VALUE(7 - ((port - 1) % 8));

        if (dev_swdrvl4_traffic_seg.port_session_id[unit-1][port-1] != 0 &&
            (dev_swdrvl4_traffic_seg.uplink_list[byte_index] & bit_value) != 0)
        {
            /* up-to-up: block -> forward
             *      uplink of the same session: forward -> forward
             *      uplink of other sessions: block -> forward
             * up-to-up: forward -> block
             *      uplink of the same session: forward -> forward
             *      uplink of other sessions: forward -> block
             */
            session_id = dev_swdrvl4_traffic_seg.port_session_id[unit-1][port-1];

            if (dev_swdrvl4_traffic_seg.is_blocking_uplink_group)
            {
                for (tmp_session_id = 1; tmp_session_id <= SYS_ADPT_PORT_TRAFFIC_SEGMENTATION_MAX_NBR_OF_SESSIONS; tmp_session_id++)
                {
                    if (tmp_session_id == session_id)
                        continue;

                    if (mode)
                    {
                        BCM_PBMP_PORT_REMOVE(
                            dev_swdrvl4_traffic_seg.session_info[tmp_session_id-1].uplink_egr_pbmp[device_id],
                            phy_port);
                    }
                    else
                    {
                        BCM_PBMP_PORT_ADD(
                            dev_swdrvl4_traffic_seg.session_info[tmp_session_id-1].uplink_egr_pbmp[device_id],
                            phy_port);
                    }
                }
            }
        }
    } /* end of for (port) */

    dev_swdrvl4_traffic_seg.is_blocking_uplink_group = mode;

    /* update chip
     */
    if (!DEV_SWDRVL4_UpdatePrivateVlan(NULL, NULL))
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_UpdatePrivateVlan failed");
        return FALSE;
    }

    return TRUE;
}/* End of DEV_SWDRVL4_SetPrivateVlanUplinkPortMode() */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPrivateVlanTrunkMode
 *------------------------------------------------------------------------------
 * FUNCTION: This function will set status of private vlan of trunk id
 * INPUT   : session_id
 *           trunk_id
 *           is_uplink
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPrivateVlanTrunkMode(UI32_T session_id, UI32_T trunk_id, BOOL_T is_uplink)
{
    dev_swdrvl4_traffic_seg.trunk_session_id[trunk_id-1] = session_id;
    dev_swdrvl4_traffic_seg.trunk_is_uplink[trunk_id-1] = is_uplink;

    /* update chip
     */
    if (!DEV_SWDRVL4_UpdatePrivateVlan(NULL, NULL))
    {
        DEV_SWDRVL4_DEV_SWDRV_ERROR_MSG("DEV_SWDRVL4_UpdatePrivateVlan failed");
        return FALSE;
    }

    return TRUE;
}
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION) */
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE) */


/*
EPR:       ES3628BT-FLF-ZZ-00589
Problem:   PortSecurity: Port security doesn't work well.
rootcasue: The operation of delete mac after enable port security will set port mac learning mode as

'BCM_PORT_LEARN_CPU|BCM_PORT_LEARN_FWD' to chip
sloution:  skip set port mac learning mode if the port not enable port security
File:      dev_amtrdrv.c  dev_swdrvl4.c  dev_swdrvl4.h
*/
BOOL_T DEV_SWDRVL4_Get_PSEC_Status(UI32_T device_id, UI32_T phy_port)
{
    if (psec_status_bmp[device_id][phy_port] & DEV_SWDRVL4_PSEC_STATUS_ENABLE)
        return TRUE;
    else
        return FALSE;
}


/*=====================================================================
 * Exported functions for Port Security
 *=====================================================================
 */
BOOL_T DEV_SWDRVL4_EnablePortSecurity(UI32_T unit_id, UI32_T port)
{
    int      error;
    UI32_T   module_id, device_id, phy_port;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;

    if ((psec_status_bmp[device_id][phy_port] & DEV_SWDRVL4_PSEC_SHUTDOWN_STATUS_ENABLE) ||
        (psec_status_bmp[device_id][phy_port] & DEV_SWDRVL4_PSEC_TRAP_STATUS_ENABLE))
    {
        error = bcm_port_learn_set((int) device_id, (int)(phy_port), BCM_PORT_LEARN_CPU);
        if ((error==0) && (static_move_to_cpu_shadow==FALSE))
        {
            error = DEV_SWDRVL4_SetStaticMacPortMoveNotification(TRUE);
            if(error == 0)
                static_move_to_cpu_shadow=TRUE;
        }

    }
    else
        error = bcm_port_learn_set((int) device_id, (int)(phy_port), 0);

    if (error == 0)
    {
        psec_status_bmp[device_id][phy_port] |= DEV_SWDRVL4_PSEC_STATUS_ENABLE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL_T DEV_SWDRVL4_DisablePortSecurity(UI32_T unit_id, UI32_T port)
{
    int      error;
    UI32_T   module_id, device_id, phy_port;
    UI32_T   set_value;
#if (SYS_CPNT_AMTR_HW_LEARN_ON_STANDALONE==TRUE)    /*AMTR HW Learning*/
    set_value = BCM_PORT_LEARN_ARL|BCM_PORT_LEARN_FWD;
#else
    set_value = BCM_PORT_LEARN_CPU|BCM_PORT_LEARN_FWD;/*AMTR SW Learning*/
#endif


    if (DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;

    error = bcm_port_learn_set((int) device_id, (int)(phy_port), set_value);

    if (error == 0)
    {
        /* check whether security action is none on all ports
         * or port secutiry is disabled on all ports
         */
        if((DEV_SWDRVL4_CheckAllPortSecActionDisabled(device_id, phy_port)==TRUE)
           && static_move_to_cpu_shadow==TRUE)
        {
            error = DEV_SWDRVL4_SetStaticMacPortMoveNotification(FALSE);
            if(error==0)
                static_move_to_cpu_shadow=FALSE;
        }
    }

    if (error == 0)
    {
        psec_status_bmp[device_id][phy_port] &= ~DEV_SWDRVL4_PSEC_STATUS_ENABLE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortSecurityMode
 *------------------------------------------------------------------------------
 * FUNCTION: This function sets port security mode
 * INPUT   : unit_id -- physically stacking unit(box) number
 *           port    -- user port in this unit
 *           security_mode -- which mode to set
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: failed
 * NOTE    : Please refere the definition in MIB header file "leaf_es3626a.h":
 *             #define VAL_portSecAction_none               1L
 *             #define VAL_portSecAction_trap               2L
 *             #define VAL_portSecAction_shutdown           3L
 *             #define VAL_portSecAction_trapAndShutdown    4L
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortSecurityMode(UI32_T unit_id,
                                       UI32_T port,
                                       UI32_T security_mode)
{
    int      error;
    UI32_T   module_id, device_id, phy_port;
    UI32_T   set_value;
#if (SYS_CPNT_AMTR_HW_LEARN_ON_STANDALONE==TRUE)    /*AMTR HW Learning*/
    set_value = BCM_PORT_LEARN_ARL|BCM_PORT_LEARN_FWD;
#else
    set_value = BCM_PORT_LEARN_CPU|BCM_PORT_LEARN_FWD;/*AMTR SW Learning*/
#endif

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;

    if (security_mode == VAL_portSecAction_none)
    {
        if (psec_status_bmp[device_id][phy_port] & DEV_SWDRVL4_PSEC_STATUS_ENABLE)
        {
            error = bcm_port_learn_set((uint32)device_id, (uint32)phy_port, 0);
        }
        else
        {
            error = bcm_port_learn_set((uint32)device_id, (uint32)phy_port, set_value);
        }

        if (error == 0)
        {
            /* check whether security action is none on all ports
             * or port secutiry is disabled on all ports
             */
            if((DEV_SWDRVL4_CheckAllPortSecActionDisabled(device_id, phy_port)==TRUE)
               && static_move_to_cpu_shadow==TRUE)
            {
                error = DEV_SWDRVL4_SetStaticMacPortMoveNotification(FALSE);
                if(error==0)
                    static_move_to_cpu_shadow=FALSE;
            }
        }

        if (error == 0)
        {
            psec_status_bmp[device_id][phy_port] &= (~(DEV_SWDRVL4_PSEC_SHUTDOWN_STATUS_ENABLE | DEV_SWDRVL4_PSEC_TRAP_STATUS_ENABLE));
        }
        else
            return FALSE;
    }
    else if ((security_mode == VAL_portSecAction_trap) || (security_mode == VAL_portSecAction_shutdown) ||
             (security_mode == VAL_portSecAction_trapAndShutdown))
    {
        if (psec_status_bmp[device_id][phy_port] & DEV_SWDRVL4_PSEC_STATUS_ENABLE)
        {
            error = bcm_port_learn_set((uint32)device_id, (uint32)phy_port, BCM_PORT_LEARN_CPU);

            if ((error==0) && static_move_to_cpu_shadow==FALSE)
            {
                error = DEV_SWDRVL4_SetStaticMacPortMoveNotification(TRUE);
                if(error == 0)
                    static_move_to_cpu_shadow=TRUE;
            }
        }
        else
        {
            error = bcm_port_learn_set((uint32)device_id, (uint32)phy_port, set_value);
        }

        if (error == 0)
        {
            if (security_mode == VAL_portSecAction_trap)
                psec_status_bmp[device_id][phy_port] |= DEV_SWDRVL4_PSEC_TRAP_STATUS_ENABLE;
            else if (security_mode == VAL_portSecAction_shutdown)
                psec_status_bmp[device_id][phy_port] |= DEV_SWDRVL4_PSEC_SHUTDOWN_STATUS_ENABLE;
            else if  (security_mode == VAL_portSecAction_trapAndShutdown)
                psec_status_bmp[device_id][phy_port] |= (DEV_SWDRVL4_PSEC_TRAP_STATUS_ENABLE | DEV_SWDRVL4_PSEC_SHUTDOWN_STATUS_ENABLE);
        }
        else
            return FALSE;
    }

    return TRUE;
}

/*=====================================================================
 * Exported functions for DiffServ
 *=====================================================================
 */

/* see swdrvl4.h prepared for Acute DiffServ design and implementation */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_GetNextIndexFromPortList
 *------------------------------------------------------------------------------
 * FUNCTION:
 * INPUT   :
 * OUTPUT  :
 * RETURN  :
 * NOTE    : None.
 *------------------------------------------------------------------------------*/
static BOOL_T DEV_SWDRVL4_GetNextIndexFromPortList (UI32_T *index,
                                                    UI8_T  port_list[SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST])
{
    I32_T byte;
    I32_T bit;
    I32_T init_byte;                /*byte: 0 1 2 ... */
    I32_T init_bit;                 /*bit:  7 6 5 ... 0*/

    if(*index >= SYS_ADPT_TOTAL_NBR_OF_LPORT)
    {
        return FALSE;
    }

    init_byte = (UI32_T)((*index)/8);
    init_bit  = 7 - ( (*index)%8 );

    for (byte = init_byte; byte < SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST; byte++)
    {
        if (port_list[byte] == 0)
        {
            init_bit = 7;
            continue;
        }

        for (bit = init_bit; bit >= 0; bit--)
        {
            if ( port_list[byte] & (1<<bit))
            {
                *index = (8*byte) + (8-bit);
                return TRUE;
            }
        }
        init_bit = 7;
    }
    return FALSE;
} /* End of DEV_SWDRVL4_GetNextIndexFromPortList() */

BOOL_T DEV_SWDRVL4_SetCosTrustMode(UI32_T unit_id,
                                       UI32_T port,
                                       UI32_T mode)
{
    UI32_T   module_id, device_id, phy_port;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;

        return DEV_SWDRVL4_SetCosTrustModeStub(device_id, phy_port, mode);
}

BOOL_T DEV_SWDRVL4_SetCosIngressCos2Dscp(UI32_T unit_id,
                                       UI32_T port,UI32_T cos,UI32_T cfi,UI32_T phb,UI32_T color)
{
    int      error;
    UI32_T   module_id, device_id, phy_port;
    color_t   temp_color = (color_t)color;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;
    if(temp_color == colorGreen)
    {
        color = bcmColorGreen;
    }
    else if(temp_color == colorYellow)
    {
        color = bcmColorYellow;
    }
    else if(temp_color == colorRed)
    {
        color = bcmColorRed;
    }

    if ((SOC_IS_RAVEN(device_id)) || (SOC_IS_FIREBOLT2(device_id)))
    {
        error = bcm_port_vlan_priority_map_set(device_id,phy_port,cos,cfi,phb,color);
    }
    else if (SOC_IS_FIREBOLT(device_id))
    {
        error = bcm_port_cfi_color_set(device_id, phy_port, cfi, color);
    }
    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
        return FALSE;
    }
    return TRUE;
}

BOOL_T DEV_SWDRVL4_SetCosIngressPre2Dscp(UI32_T unit_id,
                                       UI32_T port,UI32_T dscp,UI32_T phb,UI32_T color)
{
    int      error;
    UI32_T   module_id, device_id, phy_port;
    color_t   temp_color = (color_t)color;;
    int srccp;
    int drccp;
    int prio;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;
    srccp = dscp;
    temp_color  = color;
    prio =  (int)phb;
    drccp = (phb<<3) + color;
    if(temp_color ==colorRed)
    {
    prio = (prio |0x100);
    }
    if(temp_color ==colorYellow)
    {
    prio = (prio |0x200);
    }

    if (!soc_feature(device_id, soc_feature_dscp_map_per_port))
    {
        phy_port = -1;
    }

    error = bcm_port_dscp_map_set( device_id,phy_port,srccp,drccp, prio);

    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
        return FALSE;
    }
    return TRUE;
}

BOOL_T DEV_SWDRVL4_SetCosIngressDscp2Dscp(UI32_T unit_id,
                                       UI32_T port,UI32_T dscp,UI32_T phb,UI32_T color)
{
    int      error;
    UI32_T   module_id, device_id, phy_port;
    color_t   temp_color = (color_t)color;;
    int srccp;
    int drccp;
    int prio;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;
    srccp = (int)dscp;
    temp_color  = color;
    prio =  (int)phb;
    drccp = (phb<<3) + color;
    if(temp_color ==colorRed)
    {
        prio = (prio |0x100);
    }
    if(temp_color ==colorYellow)
    {
        prio = (prio |0x200);
    }

    if (!soc_feature(device_id, soc_feature_dscp_map_per_port))
    {
        phy_port = -1;
    }

    error = bcm_port_dscp_map_set( device_id,phy_port,srccp,drccp, prio);

    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(phy_port, error);
        return FALSE;
    }
    return TRUE;
}

BOOL_T DEV_SWDRVL4_SetCosIngressDscp2Queue(UI32_T unit_id,
                                       UI32_T port,UI32_T phb,UI32_T queue)
{
    int      error;
    UI32_T   module_id, device_id, phy_port;
    bcm_cos_queue_t temp_queue;
    bcm_cos_t priority;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;
    priority  = (bcm_cos_t)phb;
    temp_queue = (bcm_cos_queue_t)queue;

    error = bcm_cosq_port_mapping_set(device_id,phy_port,priority,temp_queue);

    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
        return FALSE;
    }
    return TRUE;
}
BOOL_T DEV_SWDRVL4_SetCosIngressDscp2Color(UI32_T unit_id,
                                       UI32_T port,UI32_T phb,UI32_T color)
{
    int      error;
    UI32_T   module_id, device_id, phy_port;
    color_t   temp_color = (color_t)color;;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;
    temp_color = color;

    if(temp_color == colorGreen)
    {
        color = bcmColorGreen;
    }
    else if(temp_color == colorYellow)
    {
        color = bcmColorYellow;
    }
    else if(temp_color == colorRed)
    {
       color = bcmColorRed;
    }

    error = bcm_port_priority_color_set (device_id,phy_port,phb, color);
    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
        return FALSE;
    }
    return TRUE;
}

BOOL_T DEV_SWDRVL4_SetCosIngressDscp2Cos(UI32_T unit_id,
                                       UI32_T port,UI32_T cos,UI32_T cfi,UI32_T phb,UI32_T color)
{
    int      error = BCM_E_NONE;
    UI32_T   module_id, device_id, phy_port;
    color_t   temp_color = (color_t)color;;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit_id, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;

    if(temp_color == colorGreen)
    {
        color = bcmColorGreen;
    }
    else if(temp_color == colorYellow)
    {
        color = bcmColorYellow;
    }
    else if(temp_color == colorRed)
    {
        color = bcmColorRed;
    }

    if ((SOC_IS_RAVEN(device_id)) || (SOC_IS_FIREBOLT2(device_id)))
    {
        error = bcm_port_vlan_priority_unmap_set(device_id,phy_port,phb,color,cos,cfi);
    }
    /* Firebolt not support this api, and there is no such feature in firebolt vxworks, eric
     */

    if (error != BCM_E_NONE)
    {
        DEV_SWDRVL4_CHIP_ERROR_MSG(-1L, error);
        return FALSE;
    }
    return TRUE;
}
/*------------------------------------------------------------------------------
 * ROUTINE NAME : DEV_SWDRVL4_HandleIPCReqMsg
 *------------------------------------------------------------------------------
 * PURPOSE:
 *    Handle the ipc request message for dev_swdrvl4.
 * INPUT:
 *    ipcmsg_p  --  input request ipc message buffer
 *
 * OUTPUT:
 *    ipcmsg_p  --  output response ipc message buffer
 *
 * RETURN:
 *    TRUE  - There is a response need to send.
 *    FALSE - There is no response to send.
 *
 * NOTES:
 *    None.
 *------------------------------------------------------------------------------
 */
BOOL_T DEV_SWDRVL4_HandleIPCReqMsg(SYSFUN_Msg_T* ipcmsg_p)
{
    DEV_SWDRVL4_PMGR_IPCMSG_T   *dev_swdrvl4_ipcmsg;

    if(ipcmsg_p==NULL)
        return FALSE;

    dev_swdrvl4_ipcmsg = (DEV_SWDRVL4_PMGR_IPCMSG_T*)ipcmsg_p->msg_buf;
#if 0
    if (SYSFUN_GET_CSC_OPERATING_MODE() != SYS_TYPE_STACKING_MASTER_MODE)
    {
        dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_PMGR_RESP_RESULT_FAIL;
        ipcmsg_p->msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE;
        return TRUE;
    }
#endif

    switch(dev_swdrvl4_ipcmsg->type.cmd)
    {
#if 0
        case DEV_SWDRVL4_IPCCMD_INIT:
            break;
#endif
        case DEV_SWDRVL4_IPCCMD_ENABLECOSMAPPING:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.enablecosmapping.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_EnableCosMapping(
                                                    dev_swdrvl4_ipcmsg->data.enablecosmapping.req.maptype
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_ENABLEPORTCOSMAPPING:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.enableportcosmapping.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_EnablePortCosMapping(
                                                    dev_swdrvl4_ipcmsg->data.enableportcosmapping.req.maptype,
                                                    dev_swdrvl4_ipcmsg->data.enableportcosmapping.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.enableportcosmapping.req.port
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_DISABLECOSMAPPING:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.disablecosmapping.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_DisableCosMapping(
                                                    dev_swdrvl4_ipcmsg->data.disablecosmapping.req.maptype
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_DISABLEPORTCOSMAPPING:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.disableportcosmapping.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_DisablePortCosMapping(
                                                    dev_swdrvl4_ipcmsg->data.disableportcosmapping.req.maptype,
                                                    dev_swdrvl4_ipcmsg->data.disableportcosmapping.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.disableportcosmapping.req.port
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETCOSMAPPING:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setcosmapping.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetCosMapping(
                                                    dev_swdrvl4_ipcmsg->data.setcosmapping.req.maptype,
                                                    dev_swdrvl4_ipcmsg->data.setcosmapping.req.value,
                                                    dev_swdrvl4_ipcmsg->data.setcosmapping.req.cos
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETPORTCOSMAPPING:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setportcosmapping.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetPortCosMapping(
                                                    dev_swdrvl4_ipcmsg->data.setportcosmapping.req.maptype,
                                                    dev_swdrvl4_ipcmsg->data.setportcosmapping.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.setportcosmapping.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setportcosmapping.req.value,
                                                    dev_swdrvl4_ipcmsg->data.setportcosmapping.req.cos
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_DELETECOSMAPPING:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.deletecosmapping.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_DeleteCosMapping(
                                                    dev_swdrvl4_ipcmsg->data.deletecosmapping.req.maptype,
                                                    dev_swdrvl4_ipcmsg->data.deletecosmapping.req.value,
                                                    dev_swdrvl4_ipcmsg->data.deletecosmapping.req.cos
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_DELETEPORTCOSMAPPING:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.deleteportcosmapping.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_DeletePortCosMapping(
                                                    dev_swdrvl4_ipcmsg->data.deleteportcosmapping.req.maptype,
                                                    dev_swdrvl4_ipcmsg->data.deleteportcosmapping.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.deleteportcosmapping.req.port,
                                                    dev_swdrvl4_ipcmsg->data.deleteportcosmapping.req.value,
                                                    dev_swdrvl4_ipcmsg->data.deleteportcosmapping.req.cos
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_ENABLEPORTINGRESSRATELIMIT:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.enableportingressratelimit.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_EnablePortIngressRateLimit(
                                                    dev_swdrvl4_ipcmsg->data.enableportingressratelimit.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.enableportingressratelimit.req.port
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_DISABLEPORTINGRESSRATELIMIT:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.disableportingressratelimit.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_DisablePortIngressRateLimit(
                                                    dev_swdrvl4_ipcmsg->data.disableportingressratelimit.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.disableportingressratelimit.req.port
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETPORTINGRESSRATELIMIT:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setportingressratelimit.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetPortIngressRateLimit(
                                                    dev_swdrvl4_ipcmsg->data.setportingressratelimit.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.setportingressratelimit.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setportingressratelimit.req.rate
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_ENABLEPORTEGRESSRATELIMIT:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.enableportegressratelimit.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_EnablePortEgressRateLimit(
                                                    dev_swdrvl4_ipcmsg->data.enableportegressratelimit.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.enableportegressratelimit.req.port
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_DISABLEPORTEGRESSRATELIMIT:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.disableportegressratelimit.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_DisablePortEgressRateLimit(
                                                    dev_swdrvl4_ipcmsg->data.disableportegressratelimit.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.disableportegressratelimit.req.port
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETPORTEGRESSRATELIMIT:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setportegressratelimit.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetPortEgressRateLimit(
                                                    dev_swdrvl4_ipcmsg->data.setportegressratelimit.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.setportegressratelimit.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setportegressratelimit.req.rate
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETEGRESSSCHEDULINGMETHOD:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setegressschedulingmethod.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetEgressSchedulingMethod(
                                                    dev_swdrvl4_ipcmsg->data.setegressschedulingmethod.req.method
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETEGRESSSPSCHEDULINGQUEUES:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setegressspschedulingqueues.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetEgressSpSchedulingQueues(
                                                    dev_swdrvl4_ipcmsg->data.setegressspschedulingqueues.req.unit_id,
                                                    dev_swdrvl4_ipcmsg->data.setegressspschedulingqueues.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setegressspschedulingqueues.req.start_queue_no,
                                                    dev_swdrvl4_ipcmsg->data.setegressspschedulingqueues.req.end_queue_no
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETPORTWRRQUEUEWEIGHT:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setportwrrqueueweight.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetPortWrrQueueWeight(
                                                    dev_swdrvl4_ipcmsg->data.setportwrrqueueweight.req.unit_id,
                                                    dev_swdrvl4_ipcmsg->data.setportwrrqueueweight.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setportwrrqueueweight.req.q_id,
                                                    dev_swdrvl4_ipcmsg->data.setportwrrqueueweight.req.weight
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_ENABLETRAFFICSEGMATATION:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.enabletrafficsegmatation.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_EnableTrafficSegmatation();
            break;
        case DEV_SWDRVL4_IPCCMD_DISABLETRAFFICSEGMATATION:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.disabletrafficsegmatation.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_DisableTrafficSegmatation();
            break;
#if 0
        case DEV_SWDRVL4_IPCCMD_SETTRAFFICSEGMATATION:
            break;
#endif
        case DEV_SWDRVL4_IPCCMD_RESETTRAFFICSEGMATATION:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.resettrafficsegmatation.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_ResetTrafficSegmatation();
            break;
        case DEV_SWDRVL4_IPCCMD_SETTRAFFICSEGMATATIONBYPORTLIST:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetTrafficSegmatationByPortlist(
                                                    dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.req.uplink_port_list,
                                                    dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.req.downlink_port_list
                                                    );
            break;
#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE)
#if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION)
        case DEV_SWDRVL4_IPCCMD_SETPRIVATEVLANPORTLISTBYSESSIONID:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetPrivateVlanPortlistBySessionId(
                                                    dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.req.session_id,
                                                    dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.req.uplink_port_list,
                                                    dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.req.downlink_port_list
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_DELETEPRIVATEVLANPORTLISTBYSESSIONID:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_DeletePrivateVlanPortlistBySessionId(
                                                    dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.req.session_id,
                                                    dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.req.uplink_port_list,
                                                    dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.req.downlink_port_list
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETPRIVATEVLANUPLINKPORTMODE:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetPrivateVlanUplinkPortMode(
                                                    dev_swdrvl4_ipcmsg->data.setprivatevlanuplinkportmode.req.mode
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETPRIVATEVLANTRUNKMODE:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.settrafficsegmatationbyportlist.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetPrivateVlanTrunkMode(
                                                    dev_swdrvl4_ipcmsg->data.settrafficsegmatationtrunkmode.req.session_id,
                                                    dev_swdrvl4_ipcmsg->data.settrafficsegmatationtrunkmode.req.trunk_id,
                                                    dev_swdrvl4_ipcmsg->data.settrafficsegmatationtrunkmode.req.is_uplink
                                                    );
            break;
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE == SYS_CPNT_PORT_TRAFFIC_SEGMENTATION_MODE_MULTIPLE_SESSION) */
#endif /* End of #if (SYS_CPNT_PORT_TRAFFIC_SEGMENTATION == TRUE) */
        case DEV_SWDRVL4_IPCCMD_ENABLEPORTSECURITY:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.enableportsecurity.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_EnablePortSecurity(
                                                    dev_swdrvl4_ipcmsg->data.enableportsecurity.req.unit_id,
                                                    dev_swdrvl4_ipcmsg->data.enableportsecurity.req.port
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_DISABLEPORTSECURITY:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.disableportsecurity.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_DisablePortSecurity(
                                                    dev_swdrvl4_ipcmsg->data.disableportsecurity.req.unit_id,
                                                    dev_swdrvl4_ipcmsg->data.disableportsecurity.req.port
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETPORTSECURITYMODE:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setportsecuritymode.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetPortSecurityMode(
                                                    dev_swdrvl4_ipcmsg->data.setportsecuritymode.req.unit_id,
                                                    dev_swdrvl4_ipcmsg->data.setportsecuritymode.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setportsecuritymode.req.security_mode
                                                    );
            break;
                /* 2008-06-02: Jinfeng.Chen: Added to support individual port scheduling method*/
                case DEV_SWDRVL4_IPCCMD_SETPORTEGRESSSCHEDULINGMETHOD:
                        ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setegressschedulingmethod.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetPortEgressSchedulingMethod(
                                                                                                        dev_swdrvl4_ipcmsg->data.setegressschedulingmethod.req.unit,
                                                                                                        dev_swdrvl4_ipcmsg->data.setegressschedulingmethod.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setegressschedulingmethod.req.method
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETCOSTRUSTMODE:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setcostrustmode.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetCosTrustMode(
                                                    dev_swdrvl4_ipcmsg->data.setcostrustmode.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.setcostrustmode.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setcostrustmode.req.mode
                                                    );
            break;
        case DEV_SWDRVL4_IPCCMD_SETCOSINGRESSCOS2DSCP:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetCosIngressCos2Dscp(
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.cos,
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.cfi,
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.phb,
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.color
                                                    );
                break;
        case DEV_SWDRVL4_IPCCMD_SETCOSINGRESSPRE2DSCP:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setcosingpre2dscp.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetCosIngressPre2Dscp(
                                                    dev_swdrvl4_ipcmsg->data.setcosingpre2dscp.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.setcosingpre2dscp.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setcosingpre2dscp.req.dscp,
                                                    dev_swdrvl4_ipcmsg->data.setcosingpre2dscp.req.phb,
                                                    dev_swdrvl4_ipcmsg->data.setcosingpre2dscp.req.color
                                                    );
                break;
        case DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2DSCP:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setcosingdscp2dscp.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetCosIngressDscp2Dscp(
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2dscp.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2dscp.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2dscp.req.dscp,
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2dscp.req.phb,
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2dscp.req.color
                                                    );
                break;
        case DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2QUEUE:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setcosingdscp2queue.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetCosIngressDscp2Queue(
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2queue.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2queue.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2queue.req.phb,
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2queue.req.queue
                                                    );
                break;
        case DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2COLOR:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setcosingdscp2color.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetCosIngressDscp2Color(
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2color.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2color.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2color.req.phb,
                                                    dev_swdrvl4_ipcmsg->data.setcosingdscp2color.req.color
                                                    );
                break;
        case DEV_SWDRVL4_IPCCMD_SETCOSINGRESSDSCP2COS:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetCosIngressDscp2Cos(
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.unit,
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.cos,
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.cfi,
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.phb,
                                                    dev_swdrvl4_ipcmsg->data.setcosingcos2dscp.req.color
                                                    );
                break;
#if (SYS_CPNT_SFLOW == TRUE)
        case DEV_SWDRVL4_IPCCMD_SETSFLOWPORTSTATUS:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setsflowportstatus.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_EnablePortSflow(
                                                    dev_swdrvl4_ipcmsg->data.setsflowportstatus.req.unit_id,
                                                    dev_swdrvl4_ipcmsg->data.setsflowportstatus.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setsflowportstatus.req.enable
                                                    );
     break;
        case DEV_SWDRVL4_IPCCMD_SETSFLOWPORTRATE:
            ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                               sizeof(dev_swdrvl4_ipcmsg->data.setsflowportrate.resp);
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetPortSflowRate(
                                                    dev_swdrvl4_ipcmsg->data.setsflowportrate.req.unit_id,
                                                    dev_swdrvl4_ipcmsg->data.setsflowportrate.req.port,
                                                    dev_swdrvl4_ipcmsg->data.setsflowportrate.req.rate
                                                    );
        break;
#endif
#if (SYS_CPNT_DOS == TRUE)
    	case DEV_SWDRVL4_IPCCMD_SETDOSPROTECTIONFILTER:
                ipcmsg_p->msg_size=DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE +
                                   sizeof(dev_swdrvl4_ipcmsg->data.setsflowportrate.resp);
                dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_SetDosProtectionFilter(
                                                        dev_swdrvl4_ipcmsg->data.setdosprotection.req.type,
                                                        dev_swdrvl4_ipcmsg->data.setdosprotection.req.arg
                                                        );
            break;
#endif
        default:
            SYSFUN_Debug_Printf("%s(): Invalid cmd(%d).\n", __FUNCTION__, (int)(ipcmsg_p->cmd));
            dev_swdrvl4_ipcmsg->type.result_bool = DEV_SWDRVL4_PMGR_RESP_RESULT_FAIL;
            ipcmsg_p->msg_size = DEV_SWDRVL4_PMGR_RESP_RESULT_SIZE;
            break;
    }
    return TRUE;
}

#if (SYS_CPNT_SFLOW==TRUE)
/***************************************************************************/
/*sFlow APIs                                                         */
/***************************************************************************/
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_EnablePortSflow
 *------------------------------------------------------------------------------
 * FUNCTION: This function will enable specific port sFlow
 * INPUT   : port -- which port to enable sFlow source
 *           sflow_type--DEV_SWDRVL4_SflowIngressPort
 *                       DEV_SWDRVL4_SflowEgressPort
 *           enable-- TRUE:enable
 *                    FALSE:disable
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_EnablePortSflow(UI32_T unit, UI32_T port,BOOL_T enable)
{
    UI32_T ingress_rate = 0;
    I32_T  error = BCM_E_NONE;
    UI32_T   module_id, device_id, phy_port;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;

    if (enable)
    {
        ingress_rate = 1;
    }
    error = bcm_port_sample_rate_set(device_id, phy_port, ingress_rate, 0);

    if (BCM_E_NONE != error)
    {
        printf("Rate set error\n");
        return FALSE;
    }

    return TRUE;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetPortSflowRate
 *------------------------------------------------------------------------------
 * FUNCTION: This function will set specific port  sFlow rate
 * INPUT   : port -- which port to set sFlow rate
 *           sflow_type--DEV_SWDRVL4_SflowIngressPort
 *                       DEV_SWDRVL4_SflowEgressPort
 *           rate--sFlow rate
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetPortSflowRate(UI32_T unit, UI32_T port, UI32_T rate)
{
    I32_T  error = BCM_E_NONE;
    UI32_T   module_id, device_id, phy_port;

    if (DEV_SWDRV_Logical2PhyDevicePortID(unit, port, &module_id, &device_id, &phy_port) == FALSE)
        return FALSE;

    error = bcm_port_sample_rate_set(device_id, phy_port, rate, 0);
    if (BCM_E_NONE != error)
    {
        printf("Rate set error\n");
        return FALSE;
    }
    return TRUE;
}


/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_GetSflowSampleCount
 *------------------------------------------------------------------------------
 * FUNCTION: This function will get the sFlow sample count
 * INPUT   : port --which port to get sflow count
 *           sflow_type--ingress sFlow/egress sFlow
 *           count--sFlow sample counter
 * OUTPUT  : None
 * RETURN  : True: Successfully, FALSE: Failed
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_GetSflowSampleCount(UI32_T port,UI32_T *count)
{
    return TRUE;
}


#endif/*#if(SYS_CPNT_SFLOW==TRUE)*/

#if (SYS_CPNT_DOS == TRUE)
/*=====================================================================
 * Exported functions for DOS Protection
 *=====================================================================
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetDosProtectionFilter
 *------------------------------------------------------------------------------
 * FUNCTION: This function will config DoS protection
 * INPUT   : type   - the type of DOS protection to config
 *           enable - TRUE to enable; FALSE to disable.
 * OUTPUT  : None
 * RETURN  : TRUE/FALSE
 * NOTE    : None
 *------------------------------------------------------------------------------*/
BOOL_T DEV_SWDRVL4_SetDosProtectionFilter(UI32_T type, BOOL_T enable)
{
    UI32_T device_id;
    BOOL_T ret = TRUE;
    int error;

    for (device_id = 0; device_id < soc_ndev; device_id++)
    {
        switch (type)
        {
            case SWDRV_DOS_FILTER_SMURF:
            	error = bcm_switch_control_set(device_id, bcmSwitchDosAttackTcpSmurf, enable);
                break;

            case SWDRV_DOS_FILTER_LAND:
            	error = bcm_switch_control_set(device_id, bcmSwitchDosAttackSipEqualDip, enable);
                break;

            case SWDRV_DOS_FILTER_TCP_SCAN:
            	error = bcm_switch_control_set(device_id, bcmSwitchDosAttackTcpFlags, enable);
                break;

            case SWDRV_DOS_FILTER_TCP_NULL_SCAN:
            	error = bcm_switch_control_set(device_id, bcmSwitchDosAttackFlagZeroSeqZero, enable);
                break;

            case SWDRV_DOS_FILTER_TCP_SYN_FIN_SCAN:
            	error = bcm_switch_control_set(device_id, bcmSwitchDosAttackTcpFlagsSF, enable);
                break;

            case SWDRV_DOS_FILTER_TCP_XMAS_SCAN:
            	error = bcm_switch_control_set(device_id, bcmSwitchDosAttackTcpFlagsFUP, enable);
                break;

            default:
                /* not supported
                 */
                error = BCM_E_UNAVAIL;
        }

    	if (error != BCM_E_NONE)
    	{
            DEV_SWDRVL4_CHIP_ERROR_MSG(type, error);
            ret = FALSE;
    	}
    } /* end of for (device_id) */

    return ret;
}
#endif /* (SYS_CPNT_DOS == TRUE) */

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_SetStaticMacPortMoveNotification
 *------------------------------------------------------------------------------
 * FUNCTION: To enable or disable static mac port move notification on chip.
 * INPUT   : is_enable - TRUE:  enable
 *                       FALSE: disable
 * OUTPUT  : None
 * RETURN  : broadcom error code
 * NOTE    : On Helix, the register to enable or disable static mac port move
 *           notification is a global setting.
 *           If other broadcom chips support per port setting, new compiler
 *           control need to be added in this file.
 *------------------------------------------------------------------------------*/
static int DEV_SWDRVL4_SetStaticMacPortMoveNotification(BOOL_T is_enable)
{
    int error, i;

    error=0;
    for(i=0; i<soc_ndev; i++)
    {
        error |= bcm_switch_control_set(i, bcmSwitchL2StaticMoveToCpu, is_enable);
    }

    return error;
}

/*------------------------------------------------------------------------------
 * ROUTINE NAME - DEV_SWDRVL4_CheckAllPortSecActionDisabled
 *------------------------------------------------------------------------------
 * FUNCTION: Check whether security action of all ports had been disabled
 * INPUT   : ignore_device_id - the given device id and phy port id
 *           ignore_phy_port  - will be ignored for the checking.
 * OUTPUT  : None
 * RETURN  : TRUE  - All security action of all ports had been disabled
 *           FALSE - At least security action of one port is enabled
 * NOTE    : On Helix, the register to enable or disable static mac port move
 *           notification is a global setting.
 *           If other broadcom chips support per port setting, new compiler
 *           control need to be added in this file.
 *------------------------------------------------------------------------------*/
static BOOL_T DEV_SWDRVL4_CheckAllPortSecActionDisabled(UI32_T ignore_device_id, UI32_T ignore_phy_port)
{
    UI32_T device_id, phy_port;
    for(device_id=0; device_id<SYS_ADPT_MAX_NBR_OF_CHIP_PER_UNIT; device_id++)
    {
        for(phy_port=0; phy_port<SYS_ADPT_MAX_NBR_OF_SWITCH_PORT_PER_CHIP; phy_port++)
        {
            if((device_id==ignore_device_id)&&(phy_port==ignore_phy_port))
                continue;

            if((psec_status_bmp[device_id][phy_port] & DEV_SWDRVL4_PSEC_STATUS_ENABLE) &&
               ((psec_status_bmp[device_id][phy_port] & DEV_SWDRVL4_PSEC_SHUTDOWN_STATUS_ENABLE) ||
                (psec_status_bmp[device_id][phy_port] & DEV_SWDRVL4_PSEC_TRAP_STATUS_ENABLE)))
                return FALSE;
        }
    }
    return TRUE;
}



#endif  /* 0 */