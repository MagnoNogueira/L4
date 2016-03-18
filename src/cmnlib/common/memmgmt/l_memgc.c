/* Module Name: L_MEMGC.C
 * Purpose:
 *
 *  L_MEMGC provides a memory management with garbage collection. This module use
 *  first-fit to allocate memory.
 *
 * Notes:
 *  This module is a pure code. It doesn't provide any mutual exclusive protection.
 *  Any package which wants to use this module, MUST provide a mutual exclusive protection
 *  for its data objects.
 *
 * History:
 *       Date       -- Modifier, Reason
 *  1.   1/21 2002  -- Created by Ryan Wang, New platform
 *
 * Copyright(C)      Accton Corporation, 2002,
 */

/* INCLUDE FILE DECLARATIONS
 */
#ifdef _DEBUG
#include <memory.h>
#include <stdio.h>
#endif

#include <stdlib.h>
#include <string.h>
#include  "sys_type.h"
#include  "l_memgc.h"
#include  "l_cvrt.h"



/* NAMING CONSTANT DECLARATIONS
 */


/* DATA TYPE DECLARACTIONS
 */

/* Note: sizeof(Handle_T) must be located at 4 bytes boundary */
typedef struct
{
    char    *buffer;
    UI32_T  *map_table;
    UI8_T   *alloc_block_no_table;
    UI32_T  total_map_no;
    UI32_T  total_block_no;
    UI32_T  block_size;
} Handle_T;


/* LOCAL FUNCTION DECLARACTION
 */
static void ConvertBnoToMap(UI32_T bno, UI64_T *in_map);



/* EXPORTED SUBPROGRAM BODY
 */

/* PURPOSE: Create a memory pool.
 * INPUT:   given_buffer    -- memory buffer to be managed
 *          buffer_length   -- length of given_buffer
 *          block_size      -- Minimun block size to be allocated
 *
 * OUTPUT  : None
 * RETURN  : handle of memory pool
 * NOTE: The minimum allocated size is block_size and the maxmum allocated
 *       size is block_size x 32
 */
UI32_T L_MEMGC_Create(void *given_buffer, UI32_T buffer_length, UI32_T block_size)
{
    /* LOCAL VARIABLES
     */
    Handle_T    handle;
    Handle_T    *hp;

    /* aligned to 4 bytes boundary */
    {
        char        *buffer;
        buffer =  (char*)L_ALIGN(given_buffer,4);
        buffer_length -= ( (UI32_T)buffer - (UI32_T)given_buffer );
        handle.buffer = buffer;
    }


    /* adjust block size & caculate total block number
     * Note: block number must be times of 8
     */
    handle.block_size =  L_ALIGN(block_size,4);
    handle.total_block_no = (buffer_length / handle.block_size) & (~7);

    if ( handle.total_block_no < 8)
        return 0;                   /* buffer too small */


    /* allocate buffer for 'map_table' (32 bits boundary) */
    handle.total_map_no = L_ALIGN(handle.total_block_no, 32)/32 + 1;




    /* allocate mgmt buffer, buffer = handle + map_table + alloc_block_no_table */
    {
        char    *buf;
        UI32_T  size;

        size =  sizeof(Handle_T) +                         /* for handle */
                handle.total_map_no * sizeof(UI32_T) +     /* for map table */
                handle.total_block_no;                     /* for alloc_block_no_table */
        buf =  malloc (size);

        if ( buf == NULL )
            return 0;

        /* construct the handler */
        hp = (Handle_T*) buf;
        *hp = handle;
        hp->map_table = (UI32_T*) (buf + sizeof(Handle_T));
        hp->alloc_block_no_table = (UI8_T*) &hp->map_table[handle.total_map_no];
    }

    /* initate the map */
    {
        UI32_T last_bno;
        last_bno = handle.total_block_no & 31;

        memset ( hp->map_table, 0xff, handle.total_map_no*sizeof(UI32_T) );
        hp->map_table [handle.total_map_no-1] = 0;
        hp->map_table [handle.total_map_no-2] = (last_bno>0)? (1<<last_bno)-1: -1;
    }


    return (UI32_T)hp;
}


/* PURPOSE: Allocate a buffer from an given pool(identify by handle)
 * INPUT:   handle  -- handle of memory pool (return value of L_MEMGC_Create() )
 *          size    -- size of buffer want to be allocated
 *
 * OUTPUT:  None
 * RETURN:  allocated buffer or NULL if fail
 * NOTE: The minimum allocated size is block_size and the maxmum allocated
 *       size is block_size x 32
 */
void *L_MEMGC_Allocate (UI32_T handle, UI32_T size)
{
    Handle_T    *hp = (Handle_T*)handle;
    UI32_T      *mp = hp->map_table;
    UI64_T      wmap;
    UI32_T      i,j;
    UI64_T      in_map0, in_map;
    UI32_T      allocated_block_no;
    UI32_T      bno;

    allocated_block_no = size/hp->block_size;

    if ( allocated_block_no > 32 )
        return NULL;       /* too large */

    ConvertBnoToMap( allocated_block_no, &in_map0);

    wmap = mp[0];
    wmap <<= 32;
    for ( i=0; i< (hp->total_map_no-1); i++)
    {
        wmap = (wmap>>32) |  ((UI64_T)mp[i+1]<<32) ;
        if ( wmap == 0 )
            continue;

        for (in_map = in_map0,j=0; j<32; j++, in_map<<=1)
        {
            if ( (in_map &wmap)== in_map )
            {
                //found; block(i,j)
                mp[i] ^= (UI32_T)in_map;
                mp[i+1] ^= (UI32_T)(in_map>>32);

                bno = i*32+j;

                hp->alloc_block_no_table [bno] = (UI8_T)allocated_block_no;
                return ( hp->buffer + (i*32+j)*hp->block_size );
            }

        }
    } /* End of for */

    return NULL;


} /* L_MEMGC_Allocate */




/* PURPOSE: Free a buffer that allocate from L_MEMGC_Allocate
 * INPUT:   handle  -- handle of memory pool (return value of L_MEMGC_Create() )
 *          buffer  -- buffer that allocate from L_MEMGC_Allocate
 * OUTPUT:  None
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_MEMGC_Free (UI32_T handle, void *buffer)
{
    Handle_T *hp = (Handle_T*)handle;
    UI32_T  blockno;
    UI64_T  map;
    UI32_T  offset, w32, bit32;

    offset = (UI32_T)buffer - (UI32_T)hp->buffer;

    blockno =  offset / hp->block_size;


    if ( blockno >= hp->total_block_no)
        return FALSE;

    w32 = blockno / 32;     /* which word */
    bit32 = blockno & 31;   /* which bit */

    if ( hp->alloc_block_no_table[blockno] == 0 ||
         hp->alloc_block_no_table[blockno] > 32 )
         return FALSE;

    ConvertBnoToMap( hp->alloc_block_no_table[blockno], &map);
    hp->alloc_block_no_table[blockno] = 0;

    map = map << bit32;

    hp->map_table [w32] |= (UI32_T) map;
    hp->map_table [w32+1] |= (UI32_T) (map>>32);

    return TRUE;

} /* L_MEMGC_Free */




static void ConvertBnoToMap(UI32_T bno, UI64_T *in_map)
{
    UI32_T map=1;

    map = (map << bno) -1;
    *in_map = (UI64_T)map;
}





#ifdef _DEBUG
#include <conio.h>
void L_MEMGC_Test()
{

UI32_T  handle, cc, size=32;
UI32_T  k=0;
char    *buf, *p, *list[100];

    buf = malloc(32*32);
    memset(buf, 0, 32*32);
    handle = L_MEMGC_Create(buf, 32*32, 32);

loop:
    switch ( cc=getch() )
    {
    case 27:
        return;

    case '0':
    case '1':
    case '2':
    case '3':
        size = 32 << (cc-0x30);
        if ( (p = L_MEMGC_Allocate (handle, size)) != NULL )
        {
            list[k++]=p;
        }
        break;
    case '4':
        if ( k>0 )
        {
            L_MEMGC_Free (handle, list[--k]);
        }
        break;
    }
    goto loop;

}

#endif

