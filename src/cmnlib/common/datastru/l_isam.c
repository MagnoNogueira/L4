
#include <string.h>
#include "sys_type.h"
#include "l_isam.h"




/* MACRO FUNCTIONS
 */
/* Note: return UI8_T* */
#if 0
static UI8_T *debug_p;
#define NODE(nidx) (debug_p=desc->node+desc->rec_len * (nidx))
#endif

#define Abs(a,b)  (((a)>(b))? (a)-(b): (b)-(a))
#define NodeLink(nidx)  *(UI16_T*)((UI32_T)desc->node_link + (desc->key_len+2)*(nidx))
#define NodeKey(nidx)  (UI8_T*)((UI32_T)desc->node_link + (desc->key_len+2)*(nidx) + 2)

/* LOCAL FUNCTIONs
 */
void L_ISAM_Dump (L_ISAM_Desc_T *desc);
static void    L_ISAM_InsertNode_X (L_ISAM_Desc_T *desc, UI32_T iidx, UI32_T pre_nidx, UI32_T nidx);
static void    L_ISAM_Adjust(L_ISAM_Desc_T *desc);
static void    Remove_Index_Entry(L_ISAM_Desc_T *desc, UI32_T iidx);
static BOOL_T  L_ISAM_Hash_Locate(L_ISAM_Desc_T *desc, UI8_T *key, UI32_T *ret_hidx);
static void    L_ISAM_Hash_Delete (L_ISAM_Desc_T *desc, UI8_T *key);
static void    L_ISAM_Hash_Insert (L_ISAM_Desc_T *desc, UI8_T*key, UI32_T nidx);



/* STATIC VARIABLES
 */
const static Index_T  empty_index_element = { NULL_INDEX, NULL_INDEX, 0};



/*****************************
 *****************************/
void L_ISAM_Init(L_ISAM_Desc_T *desc)
{
   Index_T  *ip  = desc->index;
   UI16_T   *hp  = desc->hash;
   UI32_T   i;

   /* initate index */
   desc->curr_index_nbr = 0;

   for ( i =0; i < desc->total_index_nbr; i++, ip++)
      *ip = empty_index_element;

   for ( i =0; i < desc->total_hash_nbr; i++, hp++)
      *hp = NULL_INDEX;
} /* L_ISAM_Init */



BOOL_T L_ISAM_InsertNode_1 (L_ISAM_Desc_T *desc, UI8_T *key, UI32_T nidx, UI32_T *ret_nidx)
{
   UI32_T iidx, idx, pre_nidx;

   if ( L_ISAM_Locate ( desc, key, &iidx, &idx, &pre_nidx) )
   {
      *ret_nidx = idx;
      return FALSE;     /* duplicate */
   }

	memcpy(NodeKey(nidx), key, desc->key_len);
   L_ISAM_InsertNode_X (desc, iidx, pre_nidx, nidx);
   return TRUE;
} /* L_ISAM_InsertNode_1 */




/*****************************
 *****************************/
UI32_T L_ISAM_DeleteNode (L_ISAM_Desc_T *desc, UI8_T *key)
{
   UI32_T iidx, nidx, pre_nidx;
   Index_T  *ip;

   if ( !L_ISAM_Locate ( desc, key, &iidx, &nidx, &pre_nidx) )
      return NULL_INDEX;

   L_ISAM_Hash_Delete (desc, key);

   ip = desc->index+iidx;

   ip->nbr--;

   if ( pre_nidx != NULL_INDEX)
      NodeLink(pre_nidx) = NodeLink(nidx);

   if ( nidx == ip->nidx_1st )
   {  /* del 1st */
      ip->nidx_1st = NodeLink(nidx);
      if ( ip->nbr == 0 )
         Remove_Index_Entry (desc, iidx);
   }
   else if ( nidx ==  ip->nidx_last )
      ip->nidx_last = (UI16_T) pre_nidx;
      /* END if */

   return nidx;

} /* L_ISAM_DeleteNode */



UI32_T L_ISAM_GetNextNode (L_ISAM_Desc_T *desc, UI8_T *key)
{
   UI32_T iidx, pre_nidx;
   UI32_T nidx;

   UI32_T   hidx;

   if ( L_ISAM_Hash_Locate (desc, key, &hidx) )
   {
      nidx = NodeLink ( desc->hash[hidx] );
      if ( nidx != NULL_INDEX)
         return  nidx;
   }

   /* locate position of current key */
   if ( L_ISAM_Locate( desc, key, &iidx, &nidx, &pre_nidx) )
   {
      return  NodeLink(nidx);
   }
   else if ( pre_nidx == NULL_INDEX  )
   {
      return desc->index[iidx].nidx_1st;
   }
   else
   {
      return  NodeLink (pre_nidx);
   }
} /* L_ISAM_GetNextNode */



/*****************************
 *****************************/
BOOL_T L_ISAM_Locate(L_ISAM_Desc_T *desc, UI8_T *key, UI32_T *iidx, UI32_T *nidx, UI32_T *pre_nidx)
{
   /* LOCAL VARIABLES */
   int r,m, l;
   UI32_T   i;
   Index_T  *ip;
   int    ret;

   /* BODY */
   ip = desc->index;
   m=l=0, r=desc->curr_index_nbr-1;


   /* compare 1st inode */
   if ( r == -1 )
   {
      *iidx = 0;
      *pre_nidx = NULL_INDEX;
      return FALSE;
   }
   if (  (ret = memcmp ( key, NodeKey(ip->nidx_last), desc->key_len) ) <=0 )
      goto Search_NodeList;

   /* compare last */
   if ( (ret =memcmp ( key, NodeKey(ip[r].nidx_last), desc->key_len)) ==0)
   {
      m=r;  goto Search_NodeList;
   }

   if ( ret > 0 )
   {
      *iidx = r;
      *pre_nidx = ip[r].nidx_last;
      return FALSE;
   }

   /* binary search */
   while (1)
   {
      m = (l+r) >> 1;
      if ( l==m )
      {
         m=r;  goto Search_NodeList;
      }

      ret = memcmp ( key, NodeKey(ip[m].nidx_last), desc->key_len);
      if ( ret == 0 )
         goto Search_NodeList;

      if ( ret > 0 )
         l=m;
      else
         r=m;
   } /* while */


Search_NodeList:
   ip+=m;
   *iidx = m;
   if ( m == 0 )
      *pre_nidx= NULL_INDEX;
   else
      *pre_nidx= ip[-1].nidx_last;

   for ( *nidx=ip->nidx_1st, i=0; i< ip->nbr; i++)
   {
      ret = memcmp ( key, NodeKey(*nidx), desc->key_len);
      if ( ret == 0 )
         return TRUE;
      if ( ret < 0 )
         return FALSE;
      *pre_nidx = *nidx;
      *nidx = NodeLink(*nidx);
   }
   /* should not be goto here */
   return FALSE;
} /* L_ISAM_Locate */

/* LOCAL FUNCTIONS BODY
 */



/*****************************
 *****************************/
static void L_ISAM_InsertNode_X (L_ISAM_Desc_T *desc, UI32_T iidx, UI32_T pre_nidx, UI32_T nidx)
{
   Index_T  *ip;

   L_ISAM_Hash_Insert(desc, NodeKey(nidx), nidx);

   if ( desc->curr_index_nbr == 0 )
   {
      NodeLink(nidx)= NULL_INDEX;      //7/27
      desc->curr_index_nbr = 1;
   }

   ip = desc->index+iidx;
   if ( pre_nidx == NULL_INDEX)
   {  /* insert at 1st */
      NodeLink(nidx)= ip->nidx_1st;
      ip->nidx_1st = (UI16_T)nidx;
      if ( ip->nbr == 0 )
         ip->nidx_last = (UI16_T)nidx;
   } /* END if */
   else
   {
      if ( (NodeLink(nidx) = NodeLink (pre_nidx)) == NULL_INDEX)
         ip->nidx_last = (UI16_T)nidx;

      if ( NodeLink(nidx) == ip->nidx_1st )
         ip->nidx_1st = (UI16_T)nidx;

      NodeLink (pre_nidx)= (UI16_T)nidx;
   }
   if ( ++ip->nbr > desc->N2 )
   {
      L_ISAM_Adjust(desc);
   }

} /* L_ISAM_InsertNode_X */



/*****************************
 *****************************/
static void Remove_Index_Entry(L_ISAM_Desc_T *desc, UI32_T iidx)
{
   /* LOCAL VARIABLES */
   Index_T  *ip;
   int      copy_len;

   /* BODY */
   copy_len = (desc->curr_index_nbr - iidx -1 ) * sizeof(Index_T);
   ip = desc->index + iidx;
   if ( copy_len > 0 )
      memmove ( ip,  ip+1, copy_len);

   desc->index [desc->curr_index_nbr-1] = empty_index_element;
   desc->curr_index_nbr --;

   if ( desc->curr_index_nbr > 0 )
   {
      NodeLink (desc->index [desc->curr_index_nbr-1].nidx_last) = NULL_INDEX;    //7/27
   }
} /* Remove_Index_Entry */




/*****************************
 *****************************/
static void L_ISAM_Adjust (L_ISAM_Desc_T *desc)
{
   /* LOCAL VARIABLES */
   Index_T  *ip;
   UI32_T i;
   UI32_T iidx, sum;
   UI32_T nn;              /* average node number for a list */

   /* BODY */

   for (ip = desc->index, sum=0, i=0; i<desc->curr_index_nbr; i++, ip++)
      sum += ip->nbr;

   nn = (sum+desc->total_index_nbr-1) /desc->total_index_nbr;

   for ( ip = desc->index, iidx=0;  iidx <= (UI32_T)(desc->total_index_nbr-2); iidx++, ip++)
   {
      while ( Abs(ip->nbr,nn) > desc->N1 )
      {
         if ( ip->nbr < nn )
         {
            if ( ip[1].nbr == 0 )
               return;
            if ( (UI16_T)(ip->nbr+ip[1].nbr) <= nn )
            {  /* join next list */
               ip->nbr += ip[1].nbr;

               NodeLink (ip->nidx_last) = ip[1].nidx_1st;
               ip->nidx_last = ip[1].nidx_last;
               Remove_Index_Entry(desc, (UI32_T)(iidx+1));
               continue;
            }
            else
            {  /* move some node from next list */
               UI32_T j;
               UI32_T move_no, idx;

               move_no = nn - ip->nbr;
               idx = ip[1].nidx_1st;
               for ( j=0; j<(move_no-1); j++)
                  idx = NodeLink(idx);
               //07/27 NodeLink (ip->nidx_last) = ip[1].nidx_1st;
               ip->nidx_last = (UI16_T)idx;
               ip[1].nidx_1st = NodeLink(idx);
               ip->nbr = (UI16_T)nn;
               ip[1].nbr -= (UI16_T)move_no;
               break;
            }
         } /* END if ( ip->nbr < nn )  */
         else  /* ( ip->nbr > nn ) */
         {  /* split list */
            UI32_T j;
            UI32_T idx;

            idx = ip->nidx_1st;
            for ( j=0; j<(nn-1); j++)
               idx = NodeLink(idx);

            // NodeLink (ip->nidx_last) = ip[1].nidx_1st;
            ip[1].nidx_1st = NodeLink(idx);
            if ( ip[1].nidx_last == NULL_INDEX )
            {
               ip[1].nidx_last = ip->nidx_last;
               NodeLink(ip[1].nidx_last) = NULL_INDEX;    // 7/27, it's a last record
               desc->curr_index_nbr = (UI16_T)(iidx +2);
            }

            ip[1].nbr += (ip->nbr-(UI16_T)nn);
            ip->nbr = (UI16_T)nn;
            ip->nidx_last = (UI16_T)idx;
            break;
         } /* END (ip->nbr>nn ) */
      } /* END while */
   } /* END for */
} /* L_ISAM_Adjust */



/*------------------------------------------------------------------------|
 * ROUTINE NAME - L_ISAM_HashIndex                         	              |
 *------------------------------------------------------------------------|
 * FUNCTION: get hash key based on mac address (6-byte array)             |
 * INPUT   : mac         - 6-byte-array mac address                       |
 * OUTPUT  : None                                                         |
 * RETURN  : hash key                                                     |
 * NOTE    : extract last 2 bytes in mac array, and get modulus from this |
 *           2-byte value ("mod" with AMTR_MAX_HASH_TABLE_NUM)            |
 *------------------------------------------------------------------------*/
static UI32_T L_ISAM_HashIndex(L_ISAM_Desc_T *desc, UI8_T *key)
{
   UI32_T iv;


   memcpy ( &iv, key, 4);     /* alignment consideration */

   return (UI32_T) (iv % desc->total_hash_nbr);
} /* end L_ISAM_HashIndex */





/*------------------------------------------------------------------------|
 * ROUTINE NAME - L_ISAM_Hash_Locate                                   |
 *------------------------------------------------------------------------|
 * FUNCTION: if a mac address exists in table then store exact index      |
 *           to hidx, mac_index                                     |
 * INPUT   : mac - mac address pointer (pointing to 6-byte buffer)        |
 *                                                                        |
 * OUTPUT  : hidx, mac_index                                        |
 * RETURN  : TRUE  if the mac exists already                              |
 *           FALSE if the mac cannot be found                             |
 * NOTE    : None                                                         |
 *------------------------------------------------------------------------*/
static BOOL_T L_ISAM_Hash_Locate(L_ISAM_Desc_T *desc, UI8_T *key, UI32_T *ret_hidx)
{
   UI32_T   i, hidx, nidx;

   hidx = L_ISAM_HashIndex( desc, key);

   for ( i=0; i< desc->hash_deepth;  i++ )
   {
      if ( (nidx=desc->hash[hidx]) != NULL_INDEX )
      {
         if ( memcmp (key, NodeKey(nidx), desc->key_len) == 0 )
         {
            *ret_hidx = hidx;
            return TRUE;
         }
      } /* END if */
      hidx += 17;        /* hash next function */
      if ( hidx >= desc->total_hash_nbr )
         hidx -= desc->total_hash_nbr;
   }
   return FALSE;

} /* L_ISAM_Hash_Locate */

static void    L_ISAM_Hash_Insert (L_ISAM_Desc_T *desc, UI8_T*key, UI32_T insert_nidx)
{
   UI16_T   i;
   UI32_T   hidx;
   UI32_T   empty_hidx = NULL_INDEX;
   UI32_T   nidx;

   hidx = L_ISAM_HashIndex( desc, key);

   for ( i=0; i< desc->hash_deepth;  i++ )
   {
      if ( (nidx=desc->hash[hidx]) == insert_nidx)
         return;

      if ( nidx == NULL_INDEX  &&  empty_hidx == NULL_INDEX)
         empty_hidx = hidx;

      hidx += 17;        /* hash next function */
      if ( hidx >= desc->total_hash_nbr )
         hidx -= desc->total_hash_nbr;
   }
   if ( empty_hidx != NULL_INDEX )
      desc->hash[empty_hidx] = (UI16_T)insert_nidx;
} /* L_ISAM_Hash_Insert */



static void L_ISAM_Hash_Delete (L_ISAM_Desc_T *desc, UI8_T *key)
{
   UI32_T   hidx;

   if ( L_ISAM_Hash_Locate (desc, key, &hidx) )
   {
      desc->hash[hidx] = NULL_INDEX ;
   }
} /* L_ISAM_Hash_Delete */


#if 0
int L_ISAM_memcmp( const UI8_T *buf1, const UI8_T *buf2, UI32_T count )
{
   buf1 += count-1;
   buf2 += count-1;
   for ( ; count; count--, buf1--, buf2--)
   {
       if ( *buf1 != *buf2 )
          return (int) (*buf1-*buf2);
   }
   return 0;

}
#endif


#ifdef WIN32
#include <conio.h>
#include <stdio.h>
/*****************************
 *****************************/
void L_ISAM_Dump (L_ISAM_Desc_T *desc)
{
   UI32_T   iidx;
   UI32_T   nidx;
   UI32_T   i, j;
   Index_T  *ip  = desc->index;
   UI8_T      *np;

   puts("\n\n");


   /* initate index */
   for ( iidx =0; iidx < desc->curr_index_nbr; iidx++, ip++)
   {
      printf("\n%d) ", iidx);

      for (nidx=ip->nidx_1st,i=0; i< ip->nbr; i++ )
      {
         if ( (i & 1) == 0 )
            puts("");
         np=NodeKey(nidx);
         for (j=0; j<8; j++)
         {
            printf("%02X-",np[j]);
         }
         printf("   ");
         nidx = NodeLink(nidx);
#if 0
         if ( ( (i+1) % 46 )== 0 )
            if ( getch() ==27 ) break;
#endif

      }
      //if ( getch() ==27 ) break;
   }
}
#endif







/*****************************
 *****************************/
#if 0
static void L_ISAM_Adjust1(L_ISAM_Desc_T *desc)
{
   /* LOCAL VARIABLES */
   Index_T  *ip;
   UI32_T i;
   UI32_T iidx, sum;
   UI32_T nn;              /* average node number for a list */

   /* BODY */

   for (ip = desc->index, sum=0, i=0; i<desc->curr_index_nbr; i++, ip++)
      sum += ip->nbr;

   nn = (sum+desc->total_index_nbr-1) /desc->total_index_nbr;

   for ( ip = desc->index, iidx=0;  iidx <= (UI32_T)(desc->total_index_nbr-2); iidx++, ip++)
   {
      loop:
      if ( ip->nbr < nn )
      {
         if ( ip[1].nbr == 0 )
            return;
         if ( (UI16_T)(ip->nbr+ip[1].nbr) <= nn )
         {  /* join next list */
            ip->nbr += ip[1].nbr;

            NodeLink (ip->nidx_last) = ip[1].nidx_1st;
            ip->nidx_last = ip[1].nidx_last;
            Remove_Index_Entry(desc, (UI32_T)(iidx+1));
            goto loop;
         }
         else
         {  /* move some node from next list */
            UI32_T j;
            UI32_T move_no, idx;

            move_no = nn - ip->nbr;
            idx = ip[1].nidx_1st;
            for ( j=0; j<(move_no-1); j++)
               idx = NodeLink(idx);
            NodeLink (ip->nidx_last) = ip[1].nidx_1st;
            ip->nidx_last = (UI16_T)idx;
            ip[1].nidx_1st = NodeLink(idx);
            ip->nbr = (UI16_T)nn;
            ip[1].nbr -= (UI16_T)move_no;
            continue;
         }
      } /* END if ( ip->nbr < nn )  */
      else  /* ( ip->nbr > nn ) */
      {  /* split list */
         UI32_T j;
         UI32_T idx;

         idx = ip->nidx_1st;
         for ( j=0; j<(nn-1); j++)
            idx = NodeLink(idx);

         NodeLink (ip->nidx_last) = ip[1].nidx_1st;
         ip[1].nidx_1st = NodeLink(idx);
         if ( ip[1].nidx_last == NULL_INDEX )
         {
            ip[1].nidx_last = ip->nidx_last;
            desc->curr_index_nbr = (UI16_T)(iidx +2);
         }

         ip[1].nbr += (ip->nbr-(UI16_T)nn);
         ip->nbr = (UI16_T)nn;
         ip->nidx_last = (UI16_T)idx;
         continue;
      } /* END (ip->nbr>nn ) */
   } /* END for */
} /* L_ISAM_Adjust */
#endif /* end #if 0 */

