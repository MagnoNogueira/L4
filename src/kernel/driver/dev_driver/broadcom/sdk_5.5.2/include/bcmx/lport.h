/*
 * $Id: lport.h,v 1.17 Broadcom SDK $
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:        lport.h
 * Purpose:     Logical and user port definitions for BCMX
 */

#ifndef   _BCMX_LPORT_H_
#define   _BCMX_LPORT_H_

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcmx/types.h>
#include <bcmx/lplist.h>

/****************************************************************
 * Forward typedefs
 ****************************************************************/

typedef struct _bcmx_port_s _bcmx_port_t;
typedef struct _bcmx_uport_hash_s _bcmx_uport_hash_t;


/****************************************************************
 *
 * BCMX application logical port definitions
 *
 ****************************************************************/

#define BCMX_UPORT_CAST(val) INT_TO_PTR(val)

/*
 * The mapping:  "application port -> BCMX logical port" is currently
 * a hash + doubly linked list.  It is assumed that adding/removing
 * ports is a relatively rare occurrance and that an allocation/free
 * call is acceptable during this process.
 */

#define BCMX_UPORT_HASH_COUNT 73       /* Prime */
#define BCMX_UPORT_HASH(uport)   ((PTR_TO_INT(uport)) % BCMX_UPORT_HASH_COUNT)

/*
 * Alternatively:
 *
 *    ((((PTR_TO_INT(uport)) >> 16) ^ ((PTR_TO_INT(uport)) & 0xffff)) % \
 *        BCMX_UPORT_HASH_COUNT)
 */

#define BCMX_UPORT_EQ(a, b)  ((a) == (b))
#define BCMX_UPORT_SET(dst, src)  ((dst) = (src))


/****************************************************************
 *
 * Special BCMX logical port definitions
 *
 *    INVALID:        Explicitly invalid logical port
 *    NO_SUCH_LPORT:  Alias for INVALID.
 *    LOCAL_CPU:      Special marker requiring a unit context;
 *                    Given a unit, this is the CPU port associated
 *                    with that unit.
 *    ETHER_ALL:      Indicates all ethernet ports (FE, GE, XE),
 *                    as defined in the soc driver file.
 *
 ****************************************************************/

#define BCMX_LPORT_INVALID          ((bcmx_lport_t)(BCM_GPORT_INVALID))
#define BCMX_NO_SUCH_LPORT          BCMX_LPORT_INVALID  /* compatibility */
#define BCMX_LPORT_LOCAL_CPU        ((bcmx_lport_t)(BCM_GPORT_LOCAL_CPU))
#define BCMX_LPORT_ETHER_ALL        \
    ((bcmx_lport_t)(BCM_GPORT_SPECIAL(BCMX_GPORT_SPECIAL_ETHER_ALL)))

struct _bcmx_port_s {
    int             bcm_unit;    /* Unit port belongs to */
    bcm_port_t      bcm_port;    /* Port number relative to bcm unit */
    bcmx_uport_t    uport;       /* Cookie for application lport */
    uint32          flags;       /* Flags BCMX_PORT_F_xxx */
    int             modid;       /* Module id for this lport */
    bcm_port_t      modport;     /* Module relative port (0..31) */
};

#define BCMX_PORT_F_VALID     0x1  /* This port is active */
#define BCMX_PORT_F_STACK_INT 0x2  /* Internal stack port */
#define BCMX_PORT_F_STACK_EXT 0x4  /* External stack port */
#define BCMX_PORT_F_IS_STACK_PORT(_flags) \
    ((_flags) & (BCMX_PORT_F_STACK_INT | BCMX_PORT_F_STACK_EXT))
#define BCMX_PORT_F_CPU       0x8  /* CPU port */
#define BCMX_PORT_F_HG        0x10 /* HiGig port */
#define BCMX_PORT_F_FE        0x20 /* 10/100 port */
#define BCMX_PORT_F_GE        0x40 /* 1000 port */
#define BCMX_PORT_F_XE        0x80 /* 10000 port */


/****************************************************************
 * Mapping Macros lports -> unit, port, modid, modport, flags
 ****************************************************************/
#define BCMX_LPORT_BOUNDS(_lport)   (BCM_GPORT_IS_SET(_lport))
#define BCMX_LPORT_VALID(_lport)    (bcmx_lport_valid(_lport))
#define BCMX_LPORT_BCM_UNIT(_lport) (bcmx_lport_bcm_unit(_lport))
#define BCMX_LPORT_BCM_PORT(_lport) (bcmx_lport_bcm_port(_lport))
#define BCMX_LPORT_MODID(_lport)    (bcmx_lport_modid(_lport))
#define BCMX_LPORT_MODPORT(_lport)  (bcmx_lport_modport(_lport))
#define BCMX_LPORT_FLAGS(_lport)    (bcmx_lport_flags(_lport))

/*
 * LPORT Iterator
 *
 * For running thru all lports.
 *
 * NOTE: Order of ports is based on the order in which ports were
 *       added in BCMX.
 */
#define BCMX_LPORT_FIRST            (bcmx_lport_first())
#define BCMX_LPORT_NEXT(_lport)     (bcmx_lport_next(_lport))

#define BCMX_FOREACH_LPORT(_lport)                                         \
    for (_lport = BCMX_LPORT_FIRST; _lport != BCMX_LPORT_INVALID;          \
             _lport = BCMX_LPORT_NEXT(_lport))

#define BCMX_FOREACH_QUALIFIED_LPORT(_lport, _flag)                        \
    BCMX_FOREACH_LPORT(_lport) if (BCMX_LPORT_FLAGS(_lport) & (_flag))


/****************************************************************
 * Mapping Macros uports <--> lports
 ****************************************************************/

/* Hash is allocated on BCMX initialization */
struct _bcmx_uport_hash_s {
    bcmx_uport_t uport;
    bcmx_lport_t lport;
    _bcmx_uport_hash_t *next;
    _bcmx_uport_hash_t *prev;
};

extern _bcmx_uport_hash_t *_bcmx_uport_hash[BCMX_UPORT_HASH_COUNT];

#define BCMX_UPORT_TO_LPORT(_uport)  bcmx_uport_to_lport(_uport)
#define BCMX_LPORT_TO_UPORT(_lport)  bcmx_uport_get(_lport)
#define BCMX_UPORT_GET(_lport)       BCMX_LPORT_TO_UPORT(_lport)
#define BCMX_UPORT_INVALID_DEFAULT   ((bcmx_uport_t) (-1))


/****************************************************************
 * bcmx_uport_create:
 *
 * This is the callback made by BCMX when a logical port is added
 * to the configuration.  BCMX uses this call to determine the application
 * logical port name for the port.  The application may use this as
 * an indication that a port has been added to the system, or it may
 * have already been notified by the stack task.
 *
 * The BCM Dispatch layer will already be set up at this point, so the
 * application can query it for additional information about the port
 * or system configuration if needed.  However, the application should
 * not make any BCMX calls inside this callback.
 *
 * The logical port reference is also provided in case the application
 * wants to record that.
 *
 * The application is responsible for setting the flags variable
 * properly to indicate whether this is a stack port, etc.
 *
 ****************************************************************/

typedef bcmx_uport_t (*bcmx_uport_create_f)(
    bcmx_lport_t lport,
    int unit,
    bcm_port_t port,
    uint32 flags);

typedef void (*bcmx_lport_remove_notify_f)(
    bcmx_lport_t lport,
    bcmx_uport_t uport);

extern bcmx_uport_create_f bcmx_uport_create;
extern bcmx_lport_remove_notify_f bcmx_lport_remove_notify;



/****************************************************************
 * Function Prototypes 
 ****************************************************************/

extern int bcmx_lport_init(void);

/* BCMX local CPU port reference */
extern int bcmx_lport_local_cpu_get(int bcm_unit, bcmx_lport_t *lport);


/****************************************************************
 * Mapping functions between application ports, BCMX lports and
 * BCM dispatch unit/ports.
 ****************************************************************/

/* Set/Get uport <-> lport */
extern bcmx_uport_t bcmx_uport_get(bcmx_lport_t lport);
extern int bcmx_uport_set(bcmx_lport_t lport, bcmx_uport_t uport);

/* Mapping <-- FROM BCMX lport */
extern int bcmx_lport_to_unit_port(bcmx_lport_t lport,
                                   int *unit, bcm_port_t *port);
extern int bcmx_lport_to_modid_port(bcmx_lport_t lport,
                                    int *modid, bcm_port_t *modport);
extern int bcmx_lport_to_modid(bcmx_lport_t lport, int *modid);

/* Mapping --> TO BCMX lport */
extern bcmx_lport_t bcmx_unit_port_to_lport(int unit, bcm_port_t port);
extern bcmx_lport_t bcmx_modid_port_to_lport(int modid, bcm_port_t port);
extern bcmx_lport_t bcmx_uport_to_lport(bcmx_uport_t uport);

/* Mapping, others */
extern int bcmx_modid_to_bcm_unit(int modid, int *bcm_unit);
extern int bcmx_uport_to_unit_port(bcmx_uport_t uport,
                                   int *bcm_unit, bcm_port_t *bcm_port);

/*
 * Backward compatibility
 */

/*
 * The following routines are defined in order to maintain
 * backwards compatibility and support existing macros.
 * New applications should be using functions defined above.
 */
extern int bcmx_lport_bcm_unit(bcmx_lport_t lport);
extern bcm_port_t bcmx_lport_bcm_port(bcmx_lport_t lport);
extern int bcmx_lport_modid(bcmx_lport_t lport);
extern bcm_port_t bcmx_lport_modport(bcmx_lport_t lport);
extern uint32 bcmx_lport_flags(bcmx_lport_t lport);
extern bcmx_lport_t bcmx_lport_first(void);
extern bcmx_lport_t bcmx_lport_next(bcmx_lport_t lport);
extern int bcmx_lport_valid(bcmx_lport_t lport);

/*
 * Deprecated
 * New applications should no longer use the following definitions.
 */
extern int bcmx_lport_max;                /* Set to 0 */
extern bcmx_lport_t _bcmx_lport_first;
extern bcmx_uport_t _bcmx_uport_invalid;
extern _bcmx_port_t *_bcmx_port;
#define BCMX_LPORT(_lport)               (NULL)
#define BCMX_LPORT_MAX_DEFAULT            0 /* Was 1024 */
#define bcmx_lport_map_init(_lport_max) bcmx_lport_init()
#define _bcmx_lport_next                bcmx_lport_next
#define _bcmx_uport_to_lport            bcmx_uport_to_lport


#endif /* _BCMX_LPORT_H_ */
