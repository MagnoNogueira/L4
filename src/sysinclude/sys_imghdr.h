/*----------------------------------------------------------------------
 *
 * FILE:  IMGHDR.H
 *
 * DESCRIPTION:
 *
 *  Here we define the Header Structure images in poppadom project
 *  History
 *
 *   Jason Hsue 11/21/2003      3rd Edition
 *      Add bit field structure members FamilySerialId and isUniversal.
 *   Jason Hsue 12/09/2003      4th Edition
 *      Add Multi image header and define module run time image and
 *      diag image type to support multi-image download.
 *----------------------------------------------------------------------*/

#ifndef _IMGHDR_H
#define _IMGHDR_H

#define IMGHDR_IMAGE_LOADERTYPE                 0  /* not used in actual file */
#define IMGHDR_IMAGE_RUNTIMETYPE                1
#define IMGHDR_IMAGE_DIAGNOSTICTYPE             2
#define IMGHDR_IMAGE_MODULE_RUNTIMETYPE         3
#define IMGHDR_IMAGE_MODULE_DIAGNOSTICTYPE      4

/* Each image instance contains this image header */
/* Assume byte postion of a long word is like B1B2B3B4, with B1 the Most-Significant-Byte */
typedef struct IH_S
{
   unsigned long  MagicWord;        /* IMGHDR_MAGIC_PATTERN */
   unsigned long  ImageChecksum;
   unsigned long  SWVersion;        /* B1.B2.B3:formal release number,B4:RD build nmuber */
   unsigned long  ImageLength;      /* the length does not include that of image header */
   unsigned int   FamilySerialId:5; /* A serial number for each family project          */
   unsigned int   ImageType     :11;/* Family project ID                                */
   unsigned int   ZipType       :16;/* one of ZIP_RAW, ZIP_GZIP, ZIP_PKZIP              */   
   unsigned int   FileType      :8; /* file type of runtime/diagnostic                  */
   unsigned int   isUniversal   :1; /* The universal bit to identify universal image    */   
   unsigned int   Year          :7; /* year base on 2000 year                           */
   unsigned int   Month         :8; /* month                                            */
   unsigned int   Day           :8; /* day                                              */
   unsigned long  Time;             /* Image Release Time B1:hour(0..23),B2:minute(0..59),B3:second(0..59) */
   unsigned long  HeaderChecksum;
} IH_T;


/* This structure is a base of the binding image file header.
 * It's extendable but needs to be 16 bytes alignment.
 * For instance, if 2 images combine together, the header length is 48 bytes.
 * If 3 image combine together, it still is 48 bytes length.
 * But if 4 image combine together, it will be 64 bytes length.
 */
typedef struct MULTI_IH_S
{
    unsigned long   MagicWord;      /* A magic word to represent multi image header         */
    unsigned short  ImageNumber;    /* How many images combined in this file                */
    unsigned short  HeaderLength;   /* 48 basically.  It would be 16 bytes alignment        */
    unsigned long   Date;           /* File Release Date e.g. "20031205"                    */
    unsigned long   Time;           /* File Release Time B1:hour(0..23),B2:minute(0..59),B3:second(0..59) */

    unsigned long   HeaderChecksum; /* Header checksum                                      */
    unsigned long   FileVersion;    /* Combined file version will be the same as Image 1 version  */
    struct {
        unsigned long   Version;    /* The first image version, usually this is mainboard version */
        unsigned long   Length;     /* first image length, it should be 4 bytes alignment         */
    } Image[1];
} MULTI_IH_T;


#define IMGHDR_MAGIC_PATTERN        0x1A2B3C4D
#define MULTI_IMGHDR_MAGIC_PATTERN  0x9A8B7C6D

/* Image Type constants defined here */
enum IMG_TYPE_E
{
   IMG_801 = 0,   /* 801 runtime*/
   IMG_302,       /* 302 prom code + runtime*/
   IMG_801_LDR,   /* Boot Loader*/
   IMG_801_PROM,  /* 801 prom code*/
   IMG_ALL        /*  All Banks of flash!*/
};

enum ZIP_TYPE_E
{
   ZIP_RAW = 0,   /* uncompressed*/
   ZIP_GZIP,      /* compressed with gzip*/
   ZIP_PKZIP,      /* compressed with pkzip*/
   ZIP_BZIP2
};

#define	LDR_INFO_BLOCK_ID   "LDR0"

typedef struct LDR_INFO_BLOCK_S
{
    unsigned char   identifier[4];  /* should be assigned LDR_INFO_BLOCK_ID */
    unsigned char   version[32];     /* image version major.minor.fix.build */
    unsigned long   start_addr;     /* image start address in target */
    unsigned long   length;         /* image length */
    unsigned long   checksum;       /* image checksum */
    unsigned short  date_year;
    unsigned char   date_month;
    unsigned char   date_day;
    unsigned char   reserved[4];    /* reserved */
    unsigned long   block_checksum; /* checksum for the block excludes this field */
} LDR_INFO_BLOCK_T;

#endif /* end _IMGHDR_H */
