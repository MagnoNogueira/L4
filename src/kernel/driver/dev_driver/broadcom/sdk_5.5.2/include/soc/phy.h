/*
 * $Id: phy.h,v 1.39 Broadcom SDK $
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
 * Useful constants and routines for PHY chips which
 * run with Orion (10/100/1000 Mb/s enet) and use MII, GMII,
 * or 8b/10b Serdes interfaces.
 *
 * See Also: IEEE 802.3 (1998) Local and Metropolitan Area Networks
 * Sections 22.2.4-XX
 */

#ifndef _SOC_PHY_H
#define _SOC_PHY_H

/* Standard MII Registers */

#define MII_CTRL_REG            0x00    /* MII Control Register : r/w */
#define MII_STAT_REG            0x01    /* MII Status Register: ro */ 
#define MII_PHY_ID0_REG         0x02    /* MII PHY ID register: r/w */
#define MII_PHY_ID1_REG         0x03    /* MII PHY ID register: r/w */
#define MII_ANA_REG             0x04    /* MII Auto-Neg Advertisement: r/w */
#define MII_ANP_REG             0x05    /* MII Auto-Neg Link Partner: ro */
#define MII_AN_EXP_REG          0x06    /* MII Auto-Neg Expansion: ro */
#define MII_GB_CTRL_REG         0x09    /* MII 1000Base-T control register */
#define MII_GB_STAT_REG         0x0a    /* MII 1000Base-T Status register */
#define MII_ESR_REG             0x0f    /* MII Extended Status register */

/* Non-standard MII Registers */

#define MII_ECR_REG             0x10    /* MII Extended Control Register */
#define MII_AUX_REG             0x18       /* MII Auxiliary Control/Status Register */
#define MII_ASSR_REG            0x19    /* MII Auxiliary Status Summary Reg */
#define MII_GSR_REG             0x1c    /* MII General status (BROADCOM) */
#define MII_MSSEED_REG          0x1d    /* MII Master/slave seed (BROADCOM) */
#define MII_AUX_MULTI_PHY_REG   0x1e    /* Auxiliary Multiple PHY (BROADCOM) */
#define MII_TEST2_REG           0x1f    /* MII Test reg (BROADCOM) */

#define MII_SHA_CD_CTRL_REG     0x13 /*13h-0;13h-1;13h-2;13h-3;
                                  *13h-4;13h-5;13h-6;13h-7 */
#define MII_SHA_CD_SEL_REG      0x14 
#define MII_SHA_AUX_STAT2_REG   0x1B 
#define MII_BRCM_TEST_REG       0x1F


/* MII Control Register: bit definitions */

#define MII_CTRL_FS_2500        (1 << 5) /* Force speed to 2500 Mbps */
#define MII_CTRL_SS_MSB         (1 << 6) /* Speed select, MSb */
#define MII_CTRL_CST            (1 << 7) /* Collision Signal test */
#define MII_CTRL_FD             (1 << 8) /* Full Duplex */
#define MII_CTRL_RAN            (1 << 9) /* Restart Autonegotiation */
#define MII_CTRL_IP             (1 << 10) /* Isolate Phy */
#define MII_CTRL_PD             (1 << 11) /* Power Down */
#define MII_CTRL_AE             (1 << 12) /* Autonegotiation enable */
#define MII_CTRL_SS_LSB         (1 << 13) /* Speed select, LSb */
#define MII_CTRL_LE             (1 << 14) /* Loopback enable */
#define MII_CTRL_RESET          (1 << 15) /* PHY reset */

#define MII_CTRL_SS(_x)         ((_x) & (MII_CTRL_SS_LSB|MII_CTRL_SS_MSB))
#define MII_CTRL_SS_10          0
#define MII_CTRL_SS_100         (MII_CTRL_SS_LSB)
#define MII_CTRL_SS_1000        (MII_CTRL_SS_MSB)
#define MII_CTRL_SS_INVALID     (MII_CTRL_SS_LSB | MII_CTRL_SS_MSB)
#define MII_CTRL_SS_MASK        (MII_CTRL_SS_LSB | MII_CTRL_SS_MSB)

/* 
 * MII Status Register: See 802.3, 1998 pg 544 
 */
#define MII_STAT_EXT            (1 << 0) /* Extended Registers */
#define MII_STAT_JBBR           (1 << 1) /* Jabber Detected */
#define MII_STAT_LA             (1 << 2) /* Link Active */
#define MII_STAT_AN_CAP         (1 << 3) /* Autoneg capable */
#define MII_STAT_RF             (1 << 4) /* Remote Fault */
#define MII_STAT_AN_DONE        (1 << 5) /* Autoneg complete */
#define MII_STAT_MF_PS          (1 << 6) /* Preamble suppression */
#define MII_STAT_ES             (1 << 8) /* Extended status (R15) */
#define MII_STAT_HD_100_T2      (1 << 9) /* Half duplex 100Mb/s supported */
#define MII_STAT_FD_100_T2      (1 << 10)/* Full duplex 100Mb/s supported */
#define MII_STAT_HD_10          (1 << 11)/* Half duplex 100Mb/s supported */
#define MII_STAT_FD_10          (1 << 12)/* Full duplex 100Mb/s supported */
#define MII_STAT_HD_100         (1 << 13)/* Half duplex 100Mb/s supported */
#define MII_STAT_FD_100         (1 << 14)/* Full duplex 100Mb/s supported */
#define MII_STAT_100_T4         (1 << 15)/* Full duplex 100Mb/s supported */

/*
 * MII Link Advertisment
 */
#define MII_ANA_ASF             (1 << 0)/* Advertise Selector Field */
#define MII_ANA_HD_10           (1 << 5)/* Half duplex 10Mb/s supported */
#define MII_ANA_FD_10           (1 << 6)/* Full duplex 10Mb/s supported */
#define MII_ANA_HD_100          (1 << 7)/* Half duplex 100Mb/s supported */
#define MII_ANA_FD_100          (1 << 8)/* Full duplex 100Mb/s supported */
#define MII_ANA_T4              (1 << 9)/* T4 */
#define MII_ANA_PAUSE           (1 << 10)/* Pause supported */
#define MII_ANA_ASYM_PAUSE      (1 << 11)/* Asymmetric pause supported */
#define MII_ANA_RF              (1 << 13)/* Remote fault */
#define MII_ANA_NP              (1 << 15)/* Next Page */

#define MII_ANA_ASF_802_3       (1)     /* 802.3 PHY */

/*
 * MII Link Advertisment (Clause 37) 
 */
#define MII_ANA_C37_NP          (1 << 15)  /* Next Page */
#define MII_ANA_C37_RF_OK       (0 << 12)  /* No error, link OK */
#define MII_ANA_C37_RF_LINK_FAIL (1 << 12)  /* Offline */
#define MII_ANA_C37_RF_OFFLINE  (2 << 12)  /* Link failure */
#define MII_ANA_C37_RF_AN_ERR   (3 << 12)  /* Auto-Negotiation Error */
#define MII_ANA_C37_PAUSE       (1 << 7)   /* Symmetric Pause */
#define MII_ANA_C37_ASYM_PAUSE  (1 << 8)   /* Asymmetric Pause */
#define MII_ANA_C37_HD          (1 << 6)   /* Half duplex */
#define MII_ANA_C37_FD          (1 << 5)   /* Full duplex */ 

/*
 * MII Link Partner Ability (Clause 37)
 */
#define MII_ANP_C37_NP          (1 << 15)  /* Next Page */
#define MII_ANP_C37_ACK         (1 << 14)  /* Acknowledge */
#define MII_ANP_C37_RF_OK       (0 << 12)  /* No error, link OK */
#define MII_ANP_C37_RF_LINK_FAIL (1 << 12)  /* Offline */
#define MII_ANP_C37_RF_OFFLINE  (2 << 12)  /* Link failure */
#define MII_ANP_C37_RF_AN_ERR   (3 << 12)  /* Auto-Negotiation Error */
#define MII_ANP_C37_PAUSE       (1 << 7)   /* Symmetric Pause */
#define MII_ANP_C37_ASYM_PAUSE  (1 << 8)   /* Asymmetric Pause */
#define MII_ANP_C37_HD          (1 << 6)   /* Half duplex */
#define MII_ANP_C37_FD          (1 << 5)   /* Full duplex */ 

/*
 * MII Link Partner Ability (SGMII)
 */
#define MII_ANP_SGMII_MODE      (1 << 0)   /* Link parneter in SGMII mode */
#define MII_ANP_SGMII_FD        (1 << 12)  /* SGMII duplex */
#define MII_ANP_SGMII_SS_10     (0 << 10)  /* 10Mbps SGMII */
#define MII_ANP_SGMII_SS_100    (1 << 10)  /* 100Mbps SGMII */
#define MII_ANP_SGMII_SS_1000   (2 << 10)  /* 1000Mbps SGMII */
#define MII_ANP_SGMII_SS_MASK   (3 << 10)  /* SGMII speed mask */
#define MII_ANP_SGMII_ACK       (1 << 14)  /* SGMII Ackonwledge */
#define MII_ANP_SGMII_LINK      (1 << 15)  /* SGMII Link */


/*
 * 1000Base-T Control Register
 */
#define MII_GB_CTRL_MS_MAN      (1 << 12) /* Manual Master/Slave mode */
#define MII_GB_CTRL_MS          (1 << 11) /* Master/Slave negotiation mode */
#define MII_GB_CTRL_PT          (1 << 10) /* Port type */
#define MII_GB_CTRL_ADV_1000FD  (1 << 9) /* Advertise 1000Base-T FD */
#define MII_GB_CTRL_ADV_1000HD  (1 << 8) /* Advertise 1000Base-T HD */

/*
 * 1000Base-T Status Register
 */
#define MII_GB_STAT_MS_FAULT    (1 << 15) /* Master/Slave Fault */
#define MII_GB_STAT_MS          (1 << 14) /* Master/Slave, 1 == Master */
#define MII_GB_STAT_LRS         (1 << 13) /* Local receiver status */
#define MII_GB_STAT_RRS         (1 << 12) /* Remote receiver status */
#define MII_GB_STAT_LP_1000FD   (1 << 11) /* Link partner 1000FD capable */
#define MII_GB_STAT_LP_1000HD   (1 << 10) /* Link partner 1000HD capable */
#define MII_GB_STAT_IDE         (0xff << 0) /* Idle error count */

/*
 * IEEE Extended Status Register
 */
#define MII_ESR_1000_X_FD       (1 << 15) /* 1000Base-T FD capable */
#define MII_ESR_1000_X_HD       (1 << 14) /* 1000Base-T HD capable */
#define MII_ESR_1000_T_FD       (1 << 13) /* 1000Base-T FD capable */
#define MII_ESR_1000_T_HD       (1 << 12) /* 1000Base-T FD capable */

/*
 * MII Extended Control Register (BROADCOM)
 */
#define MII_ECR_FE              (1 << 0) /* FIFO Elasticity */
#define MII_ECR_TLLM            (1 << 1) /* Three link LED mode */
#define MII_ECR_ET_IPG          (1 << 2) /* Extended XMIT IPG mode */
#define MII_ECR_FLED_OFF        (1 << 3) /* Force LED off */
#define MII_ECR_FLED_ON         (1 << 4) /* Force LED on */
#define MII_ECR_ELT             (1 << 5) /* Enable LED traffic */
#define MII_ECR_RS              (1 << 6) /* Reset Scrambler */
#define MII_ECR_BRSA            (1 << 7) /* Bypass Receive Sym. align */
#define MII_ECR_BMLT3           (1 << 8) /* Bypass MLT3 Encoder/Decoder */
#define MII_ECR_BSD             (1 << 9) /* Bypass Scramble/Descramble */
#define MII_ECR_B4B5B           (1 << 10) /* Bypass 4B/5B Encode/Decode */
#define MII_ECR_FI              (1 << 11) /* Force Interrupt */
#define MII_ECR_ID              (1 << 12) /* Interrupt Disable */
#define MII_ECR_TD              (1 << 13) /* XMIT Disable */
#define MII_ECR_DAMC            (1 << 14) /* DIsable Auto-MDI Crossover */
#define MII_ECR_10B             (1 << 15) /* 1 == 10B, 0 == GMII */

/*
 * GSR (BROADCOM)
 */
#define MII_GSR_FD              (1 << 0) /* Full duplex active */
#define MII_GSR_SI              (1 << 1) /* Speed 0-->10, 1 --> 100 */
#define MII_GSR_FORCE           (1 << 2) /* Force 0-->10, 1--> 100 */
#define MII_GSR_AN              (1 << 3) /* Autonegotiation enabled */

/*
 * Auxiliary Control/Status Register
 */
#define MII_AUX_LD          (1<<14) /* Link Integrity test disabled in PHY */
  
/*
 * Auxiliary Status Summary Register (ASSR - Broadcom BCM5400)
 */
#define MII_ASSR_PRTD           (1 << 0) /* Pause resolution/XMIT direction */
#define MII_ASSR_PRRD           (1 << 1) /* Pause resolution/RCV direction */
#define MII_ASSR_LS             (1 << 2) /* Link Status (1 == link up) */
#define MII_ASSR_LPNPA          (1 << 3) /* Link partner next page cap */
#define MII_ASSR_LPANA          (1 << 4) /* Link Partner AN capable */
#define MII_ASSR_ANPR           (1 << 5) /* Autoneg page received */
#define MII_ASSR_RF             (1 << 6) /* Remote Fault */
#define MII_ASSR_PDF            (1 << 7) /* Parallel detection fault */
#define MII_ASSR_HCD            (7 << 8) /* Current operating speed */
#define MII_ASSR_ANNPW          (1 << 11) /* Auto-neg next page wait */
#define MII_ASSR_ANABD          (1 << 12) /* Auto-neg Ability detected */
#define MII_ASSR_ANAD           (1 << 13) /* Auto-neg ACK detect */
#define MII_ASSR_ANCA           (1 << 14) /* Auto-neg complete ACK */
#define MII_ASSR_ANC            (1 << 15) /* AUto-neg complete */

#define MII_ASSR_HCD_FD_1000    (7 << 8)
#define MII_ASSR_HCD_HD_1000    (6 << 8)
#define MII_ASSR_HCD_FD_100     (5 << 8)
#define MII_ASSR_HCD_T4_100     (4 << 8)
#define MII_ASSR_HCD_HD_100     (3 << 8)
#define MII_ASSR_HCD_FD_10      (2 << 8)
#define MII_ASSR_HCD_HD_10      (1 << 8)
#define MII_ASSR_HCD_NC         (0 << 8) /* Not complete */

#define MII_TEST2_MS_SEL        (1 << 11)

/* 
 * MII Broadcom Test Register(MII_SHA_CD_CTRL_REG)
 */
#define MII_SHA_CD_CTRL_PB_STAT (0x3 << 12) /* 13-0 pair B status */
#define MII_SHA_CD_CTRL_PA_STAT (0x3 << 10) /* 13-0 pair A status */
#define MII_SHA_CD_CTRL_START   (0x1 << 2)  /* 13-0 start bit */
#define MII_SHA_CD_CTRL_PB_LEN  (0xFF << 8)  /* 13-1 pair B length */
#define MII_SHA_CD_CTRL_PA_LEN  (0xFF)       /* 13-1 pair A length */

/* 
 * MII Broadcom Test Register(MII_SHA_AUX_STAT2_REG)
 */
#define MII_SHA_AUX_STAT2_LEN   (0x7 << 12) /* Enable Shadow registers */

/* 
 * MII Broadcom Test Register(MII_BRCM_TEST_REG): bit definitions
 */
#define MII_BRCM_TEST_ENSHA         (1 << 7)    /* Enable Shadow registers */
#define MII_BRCM_TEST_REG_DEFAULT   0x000B      /* Register's default value */

/* 
 * MII Broadcom 
 * Auxiliary Multiple PHY Register(MII_AUX_MULTI_PHY_REG): bit definitions
 */
#define MII_BRCM_AUX_MULTI_PHY_SUPER_ISO (1 << 3) /* Super Isolate registers */
#define MII_BRCM_AUX_MULTI_PHY_RAN       (1 << 8) /* Restart-AN registers */

#define GOOD_CABLE_LEN_TUNING   35

#define CABLE_FUZZY_LEN1        10      /* for established good link */ 
#define CABLE_FUZZY_LEN2        1

#define MII_MSSEED_SEED         0x3ff

#define PHY_MIN_REG             0
#define PHY_MAX_REG             0x1f

/* Clause 45 MIIM */
#define PHY_C45_DEV_RESERVED    0x00
#define PHY_C45_DEV_PMA_PMD     0x01
#define PHY_C45_DEV_WIS         0x02
#define PHY_C45_DEV_PCS         0x03
#define PHY_C45_DEV_PHYXS       0x04
#define PHY_C45_DEV_DTEXS       0x05

#if 1 /* steven added */
#define PHY_BCMXGXS5_MODEL      0x19
#define PHY_MODEL(id0, id1) ((id1) >> 4 & 0x3f)

#endif


#endif  /* !_SOC_PHY_H */
