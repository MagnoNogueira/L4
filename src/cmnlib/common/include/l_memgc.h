/* Module Name: L_MEMGC.C
 * Purpose:
 *
 *  L_MEMGC provides a memory management with garbage collection. This module use
 *  first-fit to allocate memory.
 *
 * Notes:
 *  This module is a pure code, it does'nt provide any mutual protection.
 *  Any packages use this module, MUST provide mutual protection for their data objects.
 *
 * History:
 *       Date       -- Modifier,        Reason
 *  1.   1/21 2002  -- Ryan Wang        Creation
 *
 * Copyright(C)      Accton Corporation, 2002,
 *
 * Example:
 *
 *       L_MEMGC_Test()
 *       {
 *
 *       UI32_T  handle, cc, size=32;
 *       UI32_T  k=0;
 *       char    *buf, *p, *list[100];
 *
 *           buf = malloc(32*32);
 *           memset(buf, 0, 32*32);
 *           handle = L_MEMGC_Create(buf, 32*32, 32);
 *
 *       loop:
 *           switch ( cc=getch() )
 *           {
 *           case 27:
 *               return;
 *
 *           case '0':
 *           case '1':
 *           case '2':
 *           case '3':
 *               size = 32 << (cc-0x30);
 *               if ( (p = L_MEMGC_Allocate (handle, size)) != NULL )
 *               {
 *                   list[k++]=p;
 *               }
 *               break;
 *           case '4':
 *               if ( k>0 )
 *               {
 *                   L_MEMGC_Free (handle, list[--k]);
 *               }
 *               break;
 *           }
 *           goto loop;
 *
 *       }
 *
 */



#ifndef _L_MEMGC_H
#define _L_MEMGC_H


/* EXPORTED SUBPROGRAM DECLARATIONS
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
UI32_T L_MEMGC_Create(void *given_buffer, UI32_T buffer_length, UI32_T block_size);




/* PURPOSE: Allocate a buffer from an given pool(identify by handle)
 * INPUT:   handle  -- handle of memory pool (return value of L_MEMGC_Create() )
 *          size    -- size of buffer want to be allocated
 *
 * OUTPUT:  None
 * RETURN:  allocated buffer or NULL if fail
 * NOTE: The minimum allocated size is block_size and the maxmum allocated
 *       size is block_size x 32
 */
void *L_MEMGC_Allocate (UI32_T handle, UI32_T size);



/* PURPOSE: Free a buffer that allocate from L_MEMGC_Allocate
 * INPUT:   handle  -- handle of memory pool (return value of L_MEMGC_Create() )
 *          buffer  -- buffer that allocate from L_MEMGC_Allocate
 * OUTPUT:  None
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_MEMGC_Free (UI32_T handle, void *buffer);


#endif
