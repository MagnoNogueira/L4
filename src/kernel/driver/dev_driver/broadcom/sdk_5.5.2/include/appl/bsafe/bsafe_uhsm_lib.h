/*
 * Bsafe uHSM Library definitions.
 *
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
 * $Id: bsafe_uhsm_lib.h,v 1.3 Broadcom SDK $
 */

#ifndef BSAFE_UHSM_LIB_H_
#define BSAFE_UHSM_LIB_H_

#if defined(INCLUDE_BSAFE)

#include <appl/bsafe/bsafe_common.h>

/*
 * bsafe driver connect/disconnect.
 */

extern int bsafe_open(int unit, int reset);
extern int bsafe_close(int fd);





/* BSAFE - uHSM API */


/* 	API - assumptions 
 * 	- Expecting the caller to check the maximum limits, pass in the required  size buffers etc 
 * 	 (ie) bsafe api _DOES_NOT_ do any validations.
 * 	- Depending on the arguments if it is BIGNUM do pass in bignums[defined in shared/bsafe_common.h 
 *	- If not bignum all messages  or byte arrays should be in Bin array format or normal format
 */




/* 	
 *	bsafe_uhsm_start 
 * 	Input:	challenge 
 * 	Output:	session id, ksc_pub, sig_begin_r, sig_begin_s
 */
extern int bsafe_uhsm_start( int fd,
		unsigned char *challenge /* 20 bytes */, 
		unsigned short *sessionid,
		unsigned char *ksc_pub /* 256 bytes */ ,
		BSAFE_BIGNUM *sig_begin_r /* 20 bytes */, 
		BSAFE_BIGNUM *sig_begin_s /* 20 bytes */);

/* sessionids are key locations for broadsafe */

/* 
 *	bsafe_uhsm_message.
 *	Input: sessionids, numsessionids, message, message_len
 * 	Ouput:	appkeyid_r, sessionid_r
 *
 */
extern int bsafe_uhsm_message( int fd,
			unsigned short *sessionids  /* no of array elements == numsessionids; ie sessionids[numsessionids] */,
			unsigned short numsessionids /* 1 <= numsessionid <= 2 */,
			unsigned char *message,
			unsigned int message_len,
			unsigned short *appkeyid_r,
			unsigned short *sessionid_r);

/* 	
 * 	bsafe_uhsm_getpubkeys
 * 	Output: kdc_pub_out, kdi_pub_out
 *
 */
extern int bsafe_uhsm_getpubkeys( int fd,
			unsigned char *kdc_pub_out /* 256 bytes */, 
			bsafe_dsa_pub_t *kdi_pub_out /* 404 bytes [128p 128g 20q 128y] */);


/* 
 *	bsafe_uhsm_userauth_sha1pp
 *	Input: challenge
 *	Output: sig_userauth_r, sig_userauth_s
 */
extern int bsafe_uhsm_userauth_sha1pp( int fd,
				unsigned char *challenge /* 20 bytes */,
				BSAFE_BIGNUM *sig_userauth_r /* 20 bytes */, 
				BSAFE_BIGNUM *sig_userauth_s /* 20 bytes */);

/* 
 *	bsafe_uhsm_decrypt_rsaprivate
 *	Input: appkeyid, ip, p_len
 * 	Output: op
 */
extern int bsafe_uhsm_decrypt_rsaprivate( int fd,
				unsigned short appkeyid,
				unsigned char *ip , 
				unsigned int p_len,
				unsigned char *op /* len of op == p_len */ );


/* 
 *	bsafe_uhsm_decrypt_des3
 *	Input: appkeyid, iv,ip, p_len
 *	Output: op
 */
extern int bsafe_uhsm_decrypt_des3( int fd,
				unsigned short appkeyid,
				unsigned char *iv /* 8 bytes */,
				unsigned char *ip , 
				unsigned int p_len,
				unsigned char *op /* len of op == p_len */ );


/* 
 *	bsafe_rfc2313_padding_remove
 *	Input: buf, len
 *	Output: out_len
 */
extern int bsafe_rfc2313_padding_remove( unsigned char * buf,
				unsigned int len,
				unsigned int *out_len);


/* misc routines */

/* 
 *	bsafe_init_devicekey
 *	Input: uhsm_config, prod_config,auth_digest, num_row_retry, num_col_retry
 *	Output:  statistics
 */
extern int bsafe_init_devicekey( int fd,
				unsigned int uhsm_config,
				unsigned char *prod_config,/* 4 bytes */
				unsigned char *auth_digest /* 20 bytes */,
				unsigned short num_row_retry , 
				unsigned short num_col_retry,
				unsigned char *statistics /* 16 bytes */);

/* 
 *	bsafe_init_devicekey
 *	Input: runtest
 *	Output: testresult
 */
extern int bsafe_selftest( int fd,
				unsigned int runtest,
				unsigned int *testresult 
				);


/* 
 *	bsafe_kek_status
 *	Input: kekloc
 *	Output: keypolicy, keytype, ksize
 */
extern int bsafe_kek_status( int fd,
				unsigned short kekloc,
				unsigned char * keypolicy, /* 6 bytes */
				unsigned short *keytype,
				unsigned int *ksize
				);

/* 
 *	bsafe_akey_status
 *	Input: akeyloc
 *	Output: keypolicy, keytype, ksize
 */
extern int bsafe_akey_status( int fd,
				unsigned short akeyloc,
				unsigned char * keypolicy, /* 6 bytes */
				unsigned short *keytype,
				unsigned int *ksize
				);

/* 
 *	bsafe_clr_key
 *	Input: location
 *	Output: ksize
 */
extern int bsafe_clr_key( int fd,
				unsigned short location,
				unsigned short cache,
				unsigned int *ksize
				);

/* 
 *	bsafe_ld_userkey [akey]
 *	Input: akeylocation, keydata, keydata_len
 *	Output: ksize
 */
extern int bsafe_ld_userkey( int fd,
				unsigned short akeylocation,
				unsigned char * keydata,
				unsigned int keydata_len,
				unsigned int *ksize
				);

/* 
 *	bsafe_ld_cfg 
 *	Input: keypolicy, keytype, km_pub, 
 *		dlies_msg, dlies_msg_len , authsig_r, authsig_s
 *	Output: 
 */
extern int bsafe_ld_cfg( int fd,
				unsigned char *keypolicy,
				unsigned short keytype,
				bsafe_dsa_pub_t *km_pub,	/* 404 bytes p g q y */
				unsigned char *dlies_msg,
				unsigned int dlies_msg_len,
				BSAFE_BIGNUM *authsig_r,
				BSAFE_BIGNUM *authsig_s
				);



/* 
 *	bsafe_fips_clr
 *	Input: 	clr_vector  , num_retry
 *	Output: 
 */
extern int bsafe_fips_clr( int fd , 
		unsigned int clr_vector, 
		unsigned short num_retry );



/* 
 *	bsafe_user_status
 *	Input: 	
 *	Output: SALeventstatus
 */
extern int bsafe_user_status( int fd , 
		unsigned int *SALeventstatus );


/* 
 *	bsafe_user_nop
 *	Input: 	datain, data_len
 *	Output: dataout
 */
extern int bsafe_user_nop( int fd , 
		unsigned char *datain,
		unsigned int data_len, 
		unsigned char * dataout );



/* 
 *	bsafe_user_random
 *	Input: 	rngtype
 *	Output: random
 */
extern int bsafe_user_random( int fd , 
		unsigned int rngtype, 
		unsigned char * random /* 20 bytes */ );

/* 
 *	bsafe_user_sha1
 *	Input: prevlen,mode, indigest, data, data_len
 *	Output: outdigest
 */
extern int bsafe_user_sha1( int fd,
				unsigned short prevlen,
				unsigned short mode, 
				unsigned char *indigest /* 20 bytes */,	
				unsigned char *data,
				unsigned int data_len,
				unsigned char *outdigest /* 20 bytes */);

/*   
	 an application bignum  with a value of 0x0807060504030201
	 where 0x08 is the MSB and 0x01 is the LSB
*/

#define BSAFE_BN_ENDIANESS_TEST_VALUE   {0x8,0x7,0x6,0x5,0x4,0x3,0x2,0x1}

extern int bsafe_test_set_bsafe_bignum_endianess(int fd,
			BSAFE_BIGNUM * one	/* 8 bytes */);

#endif /* INCLUDE_BSAFE */

#endif /* BSAFE_UHSM_LIB_H_ */
