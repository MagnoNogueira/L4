/*---------------------------------------------------------------------
 * File_Name : l_radix.h
 *
 * Purpose   : Provide Radix operations
 *
 * History:
 *       Date       --  Modifier,   Reason
 *      2008.01.17  --  Vai Wang,   First Created.
 *
 *
 * Note    : Ported from Zebra
 *---------------------------------------------------------------------
 */
#ifndef L_RADIX_TBL_H
#define L_RADIX_TBL_H

/* INCLUDE FILE DECLARATIONS
 */
#include <sys/types.h>
#include "sys_type.h"
#include "sys_pal.h"
#include "l_prefix.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
/* Routing table top structure. */
struct route_table
{
    struct route_node *top;

    /* Table identifier. */
    u_int32_t id;
};

/* Each routing entry. */
struct route_node
{
    /* DO NOT MOVE the first 2 pointers. They are used for memory
    manager as well */
    struct route_node *link[2];
#define l_left   link[0]
#define l_right  link[1]

    /* Actual prefix of this radix. */
    L_PREFIX_T p;

    /* Tree link. */
    struct route_table *table;
    struct route_node *parent;

    /* Lock of this radix */
    u_int32_t lock;

    /* Each node of route. */
    void *info;

    /* Aggregation. */
    void *aggregate;
};

/* Routing table */
typedef struct route_table      L_RADIX_Table_T;

/* Routing entry node */
typedef struct route_node       L_RADIX_Node_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/* PURPOSE: Create a Radix table
 * INPUT:   table
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_Create (L_RADIX_Table_T *table);
L_RADIX_Table_T *L_RADIX_AllocateTable(void);


/* PURPOSE: Destroy a Radix table
 * INPUT:   table
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_Destroy (L_RADIX_Table_T *table);
void L_RADIX_FreeTable (L_RADIX_Table_T *table);


/* PURPOSE: Lock a node by increase 1 to its reference counter
 * INPUT:   node
 * OUTPUT:  none
 * RETURN:  node
 */
L_RADIX_Node_T * L_RADIX_LockNode (L_RADIX_Node_T *node);


/* PURPOSE: Unlock a node, decrease 1 to its reference counter,
 *          delete the node when it equals 0.
 * INPUT:   node
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
void L_RADIX_UnlockNode (L_RADIX_Node_T *node);


/* PURPOSE: Delete a Radix node from Radix table
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  None
 */
void L_RADIX_DeleteNode(L_RADIX_Node_T *node);


/* PURPOSE: Free a Radix node
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  None.
 */
void L_RADIX_FreeNode(L_RADIX_Node_T *node);


/* PURPOSE: Search table by given prefix, return the node if 
 *          it's found, otherwise, add a new node.
 * INPUT:   table, p
 * OUTPUT:  ret_node  -- found or new created
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_GetNode (L_RADIX_Table_T *table, 
                                L_PREFIX_T *p, 
                                L_RADIX_Node_T **ret_node);
L_RADIX_Node_T *L_RADIX_NodeGet (L_RADIX_Table_T *, L_PREFIX_T *);
L_RADIX_Node_T *L_RADIX_NodeGetIPv4(L_RADIX_Table_T *, struct pal_in4_addr *);
L_RADIX_Node_T *L_RADIX_NodeGetIPv6(L_RADIX_Table_T *, struct pal_in6_addr *);


/* PURPOSE: Get fist node and lock it.
 * INPUT:   table
 * OUTPUT:  node  -- the first node
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_Get_1st(L_RADIX_Table_T *table, L_RADIX_Node_T **node);
L_RADIX_Node_T *L_RADIX_GetTableTop (L_RADIX_Table_T *);


/* PURPOSE: Unlock current node and lock next node then output it.
 * INPUT:   node  -- current node
 * OUTPUT:  next  -- the next node
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_GetNextNode (L_RADIX_Node_T *node, L_RADIX_Node_T **next);
L_RADIX_Node_T *L_RADIX_GetNext(L_RADIX_Node_T *);


/* PURPOSE: Unlock current node and lock next node until limit.
 * INPUT:   node  -- current node
 *          limit -- the limit
 * OUTPUT:  None
 * RETURN:  next  -- the next node
 */
L_RADIX_Node_T *L_RADIX_GetNextUntil(L_RADIX_Node_T *, L_RADIX_Node_T *);


/* PURPOSE: Search table by given prefix.
 * INPUT:   table, p
 * OUTPUT:  ret_node
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_LookupNode (L_RADIX_Table_T *table, 
                                L_PREFIX_T *p, 
                                L_RADIX_Node_T **ret_node);
L_RADIX_Node_T *L_RADIX_NodeLookup (L_RADIX_Table_T *, L_PREFIX_T *);
L_RADIX_Node_T *L_RADIX_NodeLookupIPv4 (L_RADIX_Table_T *, struct pal_in4_addr *);
L_RADIX_Node_T *L_RADIX_NodeLookupIPv6 (L_RADIX_Table_T *, struct pal_in6_addr *);


/* PURPOSE: Find matched prefix
 * INPUT:   table, p
 * OUTPUT:  None
 * RETURN:  the matched node if success, otherwise return NULL
 */
L_RADIX_Node_T *L_RADIX_NodeMatch(L_RADIX_Table_T *, L_PREFIX_T *);
L_RADIX_Node_T *L_RADIX_NodeMatchIPv4 (L_RADIX_Table_T *, struct pal_in4_addr *);
L_RADIX_Node_T *L_RADIX_NodeMatchIPv6 (L_RADIX_Table_T *, struct pal_in6_addr *);


/* PURPOSE: Find matched prefix, excluding the node with the exclude prefix.
 * INPUT:   table, p
 * OUTPUT:  None
 * RETURN:  the matched node if success, otherwise return NULL
 */
L_RADIX_Node_T *L_RADIX_NodeMatchExclude (L_RADIX_Table_T *, 
					     L_PREFIX_T *,
					     L_PREFIX_T *);


/* PURPOSE: check if the table contains nodes with info set
 * INPUT:   table
 * OUTPUT:  None
 * RETURN:  return 1 if node->info is set, otherwise return 0
 */
u_char L_RADIX_IsTableHasInfo(L_RADIX_Table_T *);


/* PURPOSE: Specify an identifier for this route table.
 * INPUT:   table, id
 * OUTPUT:  None
 * RETURN:  None
 */
void L_RADIX_SetTableId(L_RADIX_Table_T *, u_int32_t);


/* PURPOSE: Dump table
 * INPUT:   t
 * OUTPUT:  None
 * RETURN:  None
 */
void L_RADIX_DumpNode(L_RADIX_Table_T *t);

#endif /* L_RADIX_TBL_H */

