#ifndef __SYSFUN_H__
#define __SYSFUN_H__

#include "sys_type.h"

#ifdef _MSC_VER
#pragma warning(disable:4100)   /* unreferenced formal parameter */
#pragma warning(disable:4127)   /* conditional expression is constant */
#endif

#define SYSFUN_TakeSem(rule_om_sem_id, SYSFUN_TIMEOUT_WAIT_FOREVER)
#define SYSFUN_GiveSem(rule_om_sem_id)
#define SYSFUN_GetSem(key, cos_om_sem_id)

#define SYSFUN_ENTER_MASTER_MODE()
#define SYSFUN_ENTER_SLAVE_MODE()
#define SYSFUN_SET_TRANSITION_MODE()
#define SYSFUN_ENTER_TRANSITION_MODE()

#define SYSFUN_GET_CSC_OPERATING_MODE()                     SYS_TYPE_STACKING_MASTER_MODE

#define SYSFUN_ENTER_MASTER_MODE_ON_SHMEM(shmem_ptr)
#define SYSFUN_ENTER_SLAVE_MODE_ON_SHMEM(shmem_ptr)
#define SYSFUN_ENTER_TRANSITION_MODE_ON_SHMEM(shmem_ptr)
#define SYSFUN_SET_TRANSITION_MODE_ON_SHMEM(shmem_ptr)

#define SYSFUN_DECLARE_CSC_ON_SHMEM
#define SYSFUN_INITIATE_CSC_ON_SHMEM(shmem_ptr)
#define SYSRSC_MGR_GetShMem(shmem_id)                       &shmem_data

#define SYSFUN_ASSERT(x)
#define SYSFUN_DECLARE_CSC

/*-------------------------------------------------------------------------
 * MACRO NAME - SYSFUN_SIZE_OF_MSG
 * ------------------------------------------------------------------------
 * PURPOSE  :   Calculate the size of the total message.
 * INPUT    :   msg_size - The size of msg_buf.
 * OUTPUT   :   None.
 * RETURN   :   The size of the total message.
 * NOTES    :   None.
 * ------------------------------------------------------------------------
 */
#define SYSFUN_SIZE_OF_MSG(msg_size) ((UI32_T)(((SYSFUN_Msg_T *)0)->msg_buf) + (UI32_T)(msg_size))

/* The definition of message for msgq.
 * NOTE: Use SYSFUN_SIZE_OF_MSG() to calculate the size of the total message.
 */
typedef struct
{
    UI32_T msg_type;         /* The type of this message.
                              * For unidirectional msg queue, the receiver can
                              *   specify type to receive the first message matched the type.
                              * For bidirectional msg queue, msg_type is reserved for storing
                              *   the id of the task that sent the request.
                              */
    UI16_T cmd;              /* User custom command id. */
    UI16_T msg_size;         /* The size of msg_buf. */
    UI8_T  msg_buf[1];       /* msg_buf: start address of buffer.
                              *          to store the message payload.
                              */
} SYSFUN_Msg_T;

#endif /* #ifndef __SYSFUN_H__ */
