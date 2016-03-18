#ifndef DEV_NICDRV_LIB_H
#define DEV_NICDRV_LIB_H

#include "sys_type.h"

static UI32_T DEV_NICDRV_LIB_PktClassId(UI32_T pkt_type) {
    return pkt_type + CPSS_NET_FIRST_USER_DEFINED_E;
}

#endif