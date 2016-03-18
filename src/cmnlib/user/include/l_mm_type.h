/* MODULE NAME:  l_mm_type.h
 * PURPOSE:
 *      Define definitions which are common to l_mm.
 * NOTES:
 *      None.
 * HISTORY
 *    2006/10/18 - Charlie Chen, Created
 *
 * Copyright(C)      Accton Corporation, 2006
 */
#ifndef L_MM_TYPE_H
#define L_MM_TYPE_H

/* INCLUDE FILE DECLARATIONS
 */

/* NAMING CONSTANT DECLARATIONS
 */
enum
{
    L_MM_CMD_ALLOCATE_TX_BUFFER = 0,
    L_MM_CMD_CONSTRUCT,
    L_MM_CMD_ADD_REF_COUNT,
    L_MM_CMD_SET_FREE_FUNC_TYPE_AND_COOKIE,
    L_MM_CMD_MOVE_PDU,
    L_MM_CMD_SHIFT_PDU,
    L_MM_CMD_RELEASE,
    L_MM_CMD_GET_BUFFER_OFFSET,
    L_MM_CMD_ALLOCATE_TX_BUFFER_FROM_DEDICATED_BUFFER,
    L_MM_CMD_BACKDOOR
};

enum {
    L_MM_BACKDOOR_CMD_GETBUFFERINFO = 0,
    L_MM_BACKDOOR_CMD_GETMREFINFO,
    L_MM_BACKDOOR_CMD_GETTOTALNBROFALLOCATEDBUFFER,
    L_MM_BACKDOOR_CMD_TOGGLEDBGMSGFLAG,
    L_MM_BACKDOOR_CMD_TOGGLEVALIDATEFREEFLAG,
    L_MM_BACKDOOR_CMD_TOGGLECHECKELAPSEDTIMEOFMALLOCBUFFERFLAG
};

/* following three defined internal buffer type is bitmap
 */
#define L_MM_DEFAULT_BUFFER_TYPE       0
#define L_MM_MREF_BUFFER_TYPE          1
#define L_MM_PT_BUFFER_TYPE            2

#define L_MM_LEADING_MAGIC_BYTE_LEN    1
#define L_MM_TRAIL_MAGIC_BYTE_LEN      4 /* should be multiple of 4 */

#define L_MM_MAGIC_BYTE_VALUE          0xAA

#define L_MM_MAX_USER_BUFFER_TYPE      0x3F

/* MACRO FUNCTION DECLARATIONS
 */
#define L_MM_GET_TRAIL_MAGIC_BYTE_ADDR(monitor_p) ((UI32_T)(monitor_p) + (monitor_p)->buf_len - L_MM_TRAIL_MAGIC_BYTE_LEN)

#define L_MM_FILL_MAGIC_BYTE(magic_byte_p, magic_byte_len) \
    memset((void*)(magic_byte_p), L_MM_MAGIC_BYTE_VALUE, magic_byte_len)

#define L_MM_FILL_LEADING_MAGIC_BYTE(mm_p) (L_MM_FILL_MAGIC_BYTE((mm_p)->magic, L_MM_LEADING_MAGIC_BYTE_LEN))
#define L_MM_FILL_TRAIL_MAGIC_BYTE(mm_p)   (L_MM_FILL_MAGIC_BYTE(L_MM_GET_TRAIL_MAGIC_BYTE_ADDR(mm_p), L_MM_TRAIL_MAGIC_BYTE_LEN))

#define L_MM_IS_VALID_LEADING_MAIC_BYTES(is_valid_p, mm_p)\
    {\
        UI8_T  *mb_p  = (UI8_T*)((mm_p)->magic);\
        UI8_T  idx;\
        *(is_valid_p)=TRUE;\
        for(idx=0; idx<L_MM_LEADING_MAGIC_BYTE_LEN; idx++)\
        {\
            if(*(mb_p++) != L_MM_MAGIC_BYTE_VALUE)\
                {*(is_valid_p)=FALSE;break;}\
        }\
    }

#define L_MM_IS_VALID_TRAIL_MAGIC_BYTES(is_valid_p, mm_p)\
    {\
        UI8_T *mb_p = (UI8_T*)L_MM_GET_TRAIL_MAGIC_BYTE_ADDR(mm_p);\
        UI8_T  idx;\
        (*is_valid_p) = TRUE;\
        for(idx=0; idx<L_MM_TRAIL_MAGIC_BYTE_LEN; idx++)\
        {\
            if( (*(mb_p++) != L_MM_MAGIC_BYTE_VALUE) )\
                {*(is_valid_p) = FALSE; break;}\
        }\
    }


#define L_MM_GET_MODULEID_FROM_USERID(user_id)  ((user_id) & 0xFF)
#define L_MM_GET_POOLID_FROM_USERID(user_id)    (((user_id)>>8) & 0x3)
#define L_MM_GET_TRACEID_FROM_USERID(user_id)   (((user_id)>>10) & 0x3F)
#define L_MM_GET_EXT_TRACEID_FROM_USERID(user_id) ((user_id)>>8)

#define L_MM_GET_APP_BUFTYPE(buf_type)                (buf_type&0x3F)
#define L_MM_CONVERT_TO_INTERNAL_BUFTYPE(buf_type)    (buf_type<<6)
#define L_MM_GET_INTERNAL_BUFTYPE(buf_type)           ((buf_type&0xC0)>>6)

#define L_MM_ERROR_RETURN_VAL          0xFFFFFFFF

enum {
    L_MM_MREF_FREE_FUN_TX_BUFFER,
    L_MM_MREF_FREE_FUN_RX_BUFFER,
    L_MM_MREF_FREE_FUN_CUSTOM,
};

enum {
    L_MM_TX_BUF_POOL_ID_STKTPLG_HBT1,
    L_MM_TX_BUF_POOL_ID_DHCP_TX,
};

/* MACRO FUNCTION DECLARATIONS
 */
/*  User_Id: used to monitor allocated buffer.
 *  user_id = trace_id(6) + pool_id(2) + module_id(8)
 *  pool_id is at user's own usage/knowledge.
 *
 *  +------------------+------------+-------------+
 *  | module_id(8bits) | pool_id(2) | trace_id(6) |
 *  +------------------+------------+-------------+
 *                     |                          |
 *                     | (Can also be viewed as   |
 *                     v  one single field      ) v
 *                     +--------------------------+
 *                     |      ext_trace_id(8)     |
 *                     +--------------------------+
 *
 *  It can be used to stand for buffer being created by L_MM_PtCreate or L_MM_Malloc
 *  or 1st PtCreate and 2nd PtCreate...
 *  or it can be combined with trace_id to form a different trace information
 *  for user's own trace purpose
 */
#define L_MM_USER_ID(modul_id,pool_id,trace_id) \
        (UI16_T)( ((modul_id) & 0xFF) | (((pool_id) & 0x3)<<8) | (((trace_id) & 0x3F)<<10) )

/* L_MM_USER_ID2 provides similar functionality with L_MM_USER_ID.
 * The difference between them is that this macro combine pool_id and trace_id
 * as a single field "trace_id".
 * However, backdoor will always show pool_id and trace_id separately.
 */
#define L_MM_USER_ID2(modul_id,ext_trace_id) \
        (UI16_T)( ((modul_id) & 0xFF) | (((ext_trace_id) & 0xFF)<<8) )

/* DATA TYPE DECLARATIONS
 */
typedef struct
{
    I32_T  buf_offset;
    UI16_T buf_len;
    UI16_T pdu_offset;
    UI16_T pdu_len;
    UI16_T free_func_type;
    void   *cookie;
    void   *cookie_params;/* anzhen.zheng, 2009-05-19, 16:51:04 */
} L_MM_Mref_ConstructArg_T;

typedef struct L_MM_Backdoor_BufferInfo_S
{
    UI32_T task_id;
    UI32_T allocation_time;
    void*  buffer_addr;
    void*  last_ref_monitor_p;
    UI16_T uid;
    UI16_T buf_len;
    UI8_T  buffer_type;
    BOOL_T is_valid_leading_magic_byte;
    BOOL_T is_valid_trail_magic_byte;
} L_MM_Backdoor_BufferInfo_T;

typedef struct L_MM_Backdoor_MrefInfo_S
{
    L_MM_Backdoor_BufferInfo_T buffer_info;
    UI16_T current_usr_id;
    UI16_T next_usr_id;
    UI16_T ref_count;
    UI16_T pdu_offset;
    UI16_T pdu_len;
} L_MM_Backdoor_MrefInfo_T;

/* Memory Monitor Structure, record monitor information in this structure.
 * The allocated buffer must have the the Mornitor Information at its head.
 * This structure should not be used outside l_mm. The reason to
 * put this structure declaration here is for the ease of evaluation
 * the value of L_MM_MREF_PREFIX_RESERVED_SIZE.
 */
struct L_MM_Monitor_S /* 28 bytes */
{
    void                    *buffer_addr;   /* address of itself, for valid checking
                                             * If this address == NULL, that means this buffer is released.
                                             * If this address !=NULL, that means this buffer is allocated and
                                             * is in monitor list.
                                             */
    struct L_MM_Monitor_S   *prev_p;
    struct L_MM_Monitor_S   *next_p;        /* link list for displaying/tracing the allocated buffer information */
                                            /* each time a buffer is allocated, a Monitor Node will be inserted to the head */
                                            /* each time a buffer is freed, the respective Monitor Node will be removed from the list */
                                            /* L_MM_Pt will use this field to make a free list */
    UI32_T                   task_id;
    UI32_T                   allocation_time;/* record the time when the buffer is created */
    UI32_T                   buf_len;        /* buffer length, buf_len = sizeof(L_MM_Monitor_S) + application record length(may include L_MM_Mref_T) + L_MM_TRAIL_MAGIC_BYTE_LEN */
    UI16_T                   uid;            /* user_id */
    UI8_T                    buffer_type;    /* bit 0 to bit 5 are used by application , for example: 0 for various size, 1 for 64 byte buffer, 2 for 128 bytes fuffer,... */
                                             /* bit 6 to bit 7 are reserved for internal use(To identify this buffer is allocated as default buffer, mref buffer or pt buffer)
                                              */
    UI8_T                    magic[L_MM_LEADING_MAGIC_BYTE_LEN]; /* magic byte, for valid checking */
};

#define L_MM_DEBUG_DUMP_MONITOR_INFO(monitor_p) \
    do {\
        if (monitor_p!=NULL)\
        {\
            UI32_T current_tick = SYSFUN_GetSysTick();\
            printk("\r\n<6>taskid=%lu,elapsed_time=%lu,buf_len=%lu,module_id=%u,pool_id=%u,trace_id=%u",\
                   monitor_p->task_id,\
                   (monitor_p->allocation_time > current_tick) ? (0xFFFFFFFF - (monitor_p->allocation_time-current_tick)) : (current_tick-monitor_p->allocation_time),\
                   monitor_p->buf_len,\
                   L_MM_GET_MODULEID_FROM_USERID(monitor_p->uid),\
                   L_MM_GET_POOLID_FROM_USERID(monitor_p->uid),\
                   L_MM_GET_TRACEID_FROM_USERID(monitor_p->uid));\
        }\
    } while (0)

#define L_MM_MREF_PARAMETER_BLOCK_LEN  3000 //16 /* size of parameter block(byte) in L_MM_Mref_Handle_T */

/* To store extra info of received frame,
 * it is used for packet forwarding.
 */
typedef struct
{
    UI16_T  inner_tag_info; /* 0 means no inner tag. */
    UI16_T  pad;
} L_MM_Mref_PktInfo_T;

typedef struct
{
    UI8_T parameter_block[L_MM_MREF_PARAMETER_BLOCK_LEN]; /* this block can be
                                                           * used by user at
                                                           * its own knowledge
                                                           */
    UI16_T              current_usr_id; /* this id indicates the moudle which is
                                         * accessing this buffer currently
                                         */
    UI16_T              next_usr_id;    /* this id indicates the next moudle
                                         * which will access this buffer
                                         */

} L_MM_Mref_Handle_T;

/*  MREF control fields in buffer's header
 *  note that the size of this structure needs to be 4-byte alignment
 *  because it will be used in l_mm_pt_xxx
 */
struct L_MM_Mref_S
{
    L_MM_Mref_Handle_T  user_handle;
    void                *buffer_addr;   /* user specified buffer */
    void                *cookie;        /* free_func pointer */
    void                *cookie_params;        /* free_func parameters pointer*/
    UI16_T              free_func_type; /* free_func type */
    UI16_T              ref_count;
    UI16_T              buf_len;
    UI16_T              pdu_offset;
    UI16_T              pdu_len;
    UI16_T              padding;
};

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

#endif    /* End of L_MM_TYPE_H */

