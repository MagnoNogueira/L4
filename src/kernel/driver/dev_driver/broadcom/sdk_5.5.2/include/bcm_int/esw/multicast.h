/*
 * $Id: multicast.h,v 1.2 Broadcom SDK $
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
 * This file contains multicast definitions internal to the BCM library.
 */

#ifndef _BCM_INT_MULTICAST_H
#define _BCM_INT_MULTICAST_H

#define _BCM_MULTICAST_TYPE_L2           1
#define _BCM_MULTICAST_TYPE_L3           2
#define _BCM_MULTICAST_TYPE_VPLS         3
#define _BCM_MULTICAST_TYPE_SUBPORT      4

#define _BCM_MULTICAST_TYPE_SHIFT        24
#define _BCM_MULTICAST_TYPE_MASK         0xff

#define _BCM_MULTICAST_ID_SHIFT          0
#define _BCM_MULTICAST_ID_MASK           0xffffff

#define _BCM_MULTICAST_IS_SET(_group_)    \
        (((_group_) >> _BCM_MULTICAST_TYPE_SHIFT) != 0)

#define _BCM_MULTICAST_GROUP_SET(_group_, _type_, _id_) \
    ((_group_) = ((_type_) << _BCM_MULTICAST_TYPE_SHIFT)  | \
     (((_id_) & _BCM_MULTICAST_ID_MASK) << _BCM_MULTICAST_ID_SHIFT))

#define _BCM_MULTICAST_ID_GET(_group_) \
    (((_group_) >> _BCM_MULTICAST_ID_SHIFT) & _BCM_MULTICAST_ID_MASK)

#define _BCM_MULTICAST_TYPE_GET(_group_) \
    (((_group_) >> _BCM_MULTICAST_TYPE_SHIFT) & _BCM_MULTICAST_TYPE_MASK)

#define _BCM_MULTICAST_IS_L2(_group_)    \
        (((_group_) >> _BCM_MULTICAST_TYPE_SHIFT) == _BCM_MULTICAST_TYPE_L2)

#define _BCM_MULTICAST_IS_L3(_group_)    \
        (((_group_) >> _BCM_MULTICAST_TYPE_SHIFT) == _BCM_MULTICAST_TYPE_L3)

#define _BCM_MULTICAST_IS_VPLS(_group_)    \
        (((_group_) >> _BCM_MULTICAST_TYPE_SHIFT) == _BCM_MULTICAST_TYPE_VPLS)

#define _BCM_MULTICAST_IS_SUBPORT(_group_)    \
        (((_group_) >> _BCM_MULTICAST_TYPE_SHIFT) == _BCM_MULTICAST_TYPE_SUBPORT)

#endif	/* !_BCM_INT_MULTICAST_H */
