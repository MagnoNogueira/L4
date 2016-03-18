/*---------------------------------------------------------------------
 * File_Name : l_radix.c
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

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"
#include "sys_cpnt.h"
#include "sys_pal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "l_prefix.h"
#include "l_radix.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */

/* STATIC VARIABLE DECLARATIONS
 */
/* Utility mask array. */
static const UI8_T maskbit[] = 
{
    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
};

/* LOCAL FUNCTION DECLARATIONS
 */
static L_RADIX_Node_T *L_RADIX_NewNode(void);
static L_RADIX_Node_T *L_RADIX_CreateNode(L_RADIX_Table_T *table, 
                                                    struct prefix *prefix);
static void L_RADIX_GenerateCommonPrefix(struct prefix *n, struct prefix *p,
                                    struct prefix *new_p);
static int L_RADIX_CheckBit(UI8_T *prefix, UI8_T prefixlen);
static void L_RADIX_SetLink(L_RADIX_Node_T *node, L_RADIX_Node_T *new_node);

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */

/* PURPOSE: Create a Radix table
 * INPUT:   table
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_Create (L_RADIX_Table_T *table)
{
    if (NULL == table)
        return FALSE;

    table->top = NULL;
    return TRUE;
}


/* PURPOSE: Allocate memory to create a Radix table
 * INPUT:   None
 * OUTPUT:  none
 * RETURN:  rt
 */
L_RADIX_Table_T *L_RADIX_AllocateTable(void)
{
    L_RADIX_Table_T *rt;

    rt = (L_RADIX_Table_T *)malloc (sizeof (L_RADIX_Table_T));
    if (NULL != rt)
        memset(rt, 0, sizeof (L_RADIX_Table_T));
    
    return rt;
}


/* PURPOSE: Free memory space of a Radix table
 * INPUT:   table
 * OUTPUT:  none
 * RETURN:  None
 */
void L_RADIX_FreeTable (L_RADIX_Table_T *table)
{
    L_RADIX_Destroy(table);
}


/* PURPOSE: Destroy a Radix table
 * INPUT:   table
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_Destroy (L_RADIX_Table_T *table)
{
    L_RADIX_Node_T *tmp_node;
    L_RADIX_Node_T *node;

    if (NULL == table)
        return FALSE;

    node = table->top;
    while(node != NULL)
    {
        if (node->l_left != NULL)
        {
            node = node->l_left;
            continue;
        }
        if (node->l_right != NULL)
        {
            node = node->l_right;
            continue;
        }

        tmp_node = node;
        node = node->parent;

        if (node != NULL)
        {
            if (node->l_left == tmp_node)
                node->l_left = NULL;
            else
                node->l_right = NULL;

            L_RADIX_FreeNode(tmp_node);
        }
        else
        {
            L_RADIX_FreeNode(tmp_node);
            break;
        }
    }

    free(table);
    return TRUE;
}


/* PURPOSE: Lock a node by increase 1 to its reference counter
 * INPUT:   node
 * OUTPUT:  none
 * RETURN:  node
 */
L_RADIX_Node_T *L_RADIX_LockNode (L_RADIX_Node_T *node)
{
    node->lock++;
    return node;
}


/* PURPOSE: Unlock a node, decrease 1 to its reference counter,
 *          delete the node when it equals 0.
 * INPUT:   node
 * OUTPUT:  none
 * RETURN:  TRUE/FALSE
 */
void L_RADIX_UnlockNode (L_RADIX_Node_T *node)
{
    node->lock--;

    if (0 == node->lock)
        L_RADIX_DeleteNode(node);
}


/* PURPOSE: Search table by given prefix, return the node if 
 *          it's found, otherwise, add a new node.
 * INPUT:   table, p
 * OUTPUT:  ret_node  -- found or new created
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_GetNode (L_RADIX_Table_T *table, 
                                L_PREFIX_T *p, 
                                L_RADIX_Node_T **ret_node)
{
    L_RADIX_Node_T *new_node;
    L_RADIX_Node_T *node;
    L_RADIX_Node_T *match;

    match = NULL;
    node = table->top;
    /* Radix search */
    while (node && 
            node->p.prefixlen <= p->prefixlen &&
            L_PREFIX_Match (&node->p, p))
    {
        if (node->p.prefixlen == p->prefixlen)
        {
            L_RADIX_LockNode(node);
            *ret_node = node;
            return TRUE;
        }

        match = node;
        node = node->link[L_RADIX_CheckBit(&p->u.prefix, node->p.prefixlen)];
    }

    /* Create a new node */
    if (NULL == node)
    {
        new_node = L_RADIX_CreateNode(table, p);
        if (NULL == new_node)
            return FALSE;
        
        if (match)
            L_RADIX_SetLink(match, new_node);
        else
            table->top = new_node;
    }
    else
    {
        new_node = L_RADIX_NewNode();
        if (NULL == new_node)
            return FALSE;
        
        L_RADIX_GenerateCommonPrefix(&node->p, p, &new_node->p);
        new_node->p.family = p->family;
        new_node->table = table;
        L_RADIX_SetLink(new_node, node);

        if (match)
            L_RADIX_SetLink(match, new_node);
        else
            table->top = new_node;

        if (new_node->p.prefixlen != p->prefixlen)
        {
            match = new_node;
            new_node = L_RADIX_CreateNode(table, p);

            if (new_node == NULL)
            {
                /* Delete the "match" (above created "new") node */
                L_RADIX_DeleteNode (match);
                return FALSE;
            }
            
            L_RADIX_SetLink(match, new_node);
        }
    }

    L_RADIX_LockNode(new_node);
    *ret_node = new_node;

    return TRUE;
}


/* PURPOSE: Search table by given prefix, return the node if 
 *          it's found, otherwise, add a new node.
 * INPUT:   table, p
 * OUTPUT:  None
 * RETURN:  node/new  -- found or new created
 */
L_RADIX_Node_T *L_RADIX_NodeGet (L_RADIX_Table_T *table, L_PREFIX_T *p)
{
    L_RADIX_Node_T *new;
    L_RADIX_Node_T *node;
    L_RADIX_Node_T *match;

    match = NULL;
    node = table->top;
    while (node && node->p.prefixlen <= p->prefixlen && 
            L_PREFIX_Match (&node->p, p))
    {
        if (node->p.prefixlen == p->prefixlen)
        {
            L_RADIX_LockNode(node);
            return node;
        }
        match = node;
        node = node->link[L_RADIX_CheckBit(&p->u.prefix, node->p.prefixlen)];
    }

    if (node == NULL)
    {
        new = L_RADIX_CreateNode (table, p);

        if (new == NULL)
            return NULL;

        if (match)
            L_RADIX_SetLink (match, new);
        else
            table->top = new;
    }
    else
    {
        new = L_RADIX_NewNode();

        if (new == NULL)
            return NULL;

        L_RADIX_GenerateCommonPrefix (&node->p, p, &new->p);
        new->p.family = p->family;
        new->table = table;
        L_RADIX_SetLink (new, node);

        if (match)
            L_RADIX_SetLink (match, new);
        else
            table->top = new;

        if (new->p.prefixlen != p->prefixlen)
        {
            match = new;
            new = L_RADIX_CreateNode (table, p);

            if (new == NULL)
            {
                /* Delete the "match" (above created "new") node */
                L_RADIX_DeleteNode (match);
                return NULL;
            }

            L_RADIX_SetLink (match, new);
        }
    }
    L_RADIX_LockNode (new);

    return new;
}


/* PURPOSE: Search table by given IPv4 address, return the node if 
 *          it's found, otherwise, add a new node.
 * INPUT:   table, addr
 * OUTPUT:  None
 * RETURN:  node/new  -- found or new created
 */
L_RADIX_Node_T *L_RADIX_NodeGetIPv4(L_RADIX_Table_T *table, struct pal_in4_addr *addr)
{
    L_PREFIX_T p;

    memset(&p, 0, sizeof (L_PREFIX_T));
    p.family = AF_INET;
    p.prefixlen = IPV4_MAX_PREFIXLEN;
    p.u.prefix4 = *addr;

    return L_RADIX_NodeGet(table, &p);
}



L_RADIX_Node_T *L_RADIX_NodeGetIPv6(L_RADIX_Table_T *table, struct pal_in6_addr *addr)
{
#if (SYS_CPNT_INET6 == TRUE)
    L_PREFIX_T p;

    memset(&p, 0, sizeof (L_PREFIX_T));
    p.family = AF_INET6;
    p.prefixlen = IPV6_MAX_PREFIXLEN;
    p.u.prefix6 = *addr;

    return L_RADIX_NodeGet(table, &p);
#else
    return NULL;
#endif /* SYS_CPNT_INET6 */
}


/* PURPOSE: Get fist node and lock it.
 * INPUT:   table
 * OUTPUT:  node  -- the first node
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_Get_1st(L_RADIX_Table_T *table, L_RADIX_Node_T **node)
{
    if (table->top == NULL)
        return FALSE;

    /* Lock the top node and return it. */
    L_RADIX_LockNode (table->top);
    *node = table->top;

    return TRUE;
}


/* PURPOSE: Get the top node and lock it.
 * INPUT:   table
 * OUTPUT:  None
 * RETURN:  table->top
 */
L_RADIX_Node_T *L_RADIX_GetTableTop (L_RADIX_Table_T *table)
{
    /* If there is no node in the routing table return NULL. */
    if (table == NULL || table->top == NULL)
        return NULL;

    /* Lock the top node and return it. */
    L_RADIX_LockNode (table->top);
    return table->top;
}


/* PURPOSE: Unlock current node and lock next node then output it.
 * INPUT:   node  -- current node
 * OUTPUT:  next  -- the next node
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_GetNextNode (L_RADIX_Node_T *node, L_RADIX_Node_T **next)
{
    L_RADIX_Node_T *next_node;
    L_RADIX_Node_T *start_node;

    if (node->l_left)
    {
        next_node = node->l_left;
        L_RADIX_LockNode(next_node);
        L_RADIX_UnlockNode(node);
        *next = next_node;
        return TRUE;
    }
    if (node->l_right)
    {
        next_node = node->l_right;
        L_RADIX_LockNode(next_node);
        L_RADIX_UnlockNode(node);
        *next = next_node;
        return TRUE;
    }

    start_node = node;
    while (node->parent)
    {
        if (node->parent->l_left == node && node->parent->l_right)
        {
            next_node = node->parent->l_right;
            L_RADIX_LockNode(next_node);
            L_RADIX_UnlockNode(start_node);
            *next = next_node;
            return TRUE;
        }

        node = node->parent;
    }

    L_RADIX_UnlockNode(start_node);
    return FALSE;
}


/* PURPOSE: Unlock current node and lock next node then return it.
 * INPUT:   node  -- current node
 * OUTPUT:  None
 * RETURN:  next  -- the next node
 */
L_RADIX_Node_T *L_RADIX_GetNext(L_RADIX_Node_T *node)
{
    L_RADIX_Node_T *next;
    L_RADIX_Node_T *start;

    /* Node may be deleted from route_unlock_node so we have to preserve
     * next node's pointer. 
     */
    if (node->l_left)
    {
        next = node->l_left;
        L_RADIX_LockNode (next);
        L_RADIX_UnlockNode (node);
        return next;
    }
    if (node->l_right)
    {
        next = node->l_right;
        L_RADIX_LockNode (next);
        L_RADIX_UnlockNode (node);
        return next;
    }

    start = node;
    while (node->parent)
    {
        if (node->parent->l_left == node && node->parent->l_right)
        {
            next = node->parent->l_right;
            L_RADIX_LockNode (next);
            L_RADIX_UnlockNode (start);
            return next;
        }
        node = node->parent;
    }
    L_RADIX_UnlockNode (start);
    return NULL;
}


/* PURPOSE: Unlock current node and lock next node until limit.
 * INPUT:   node  -- current node
 *          limit -- the limit
 * OUTPUT:  None
 * RETURN:  next  -- the next node
 */
L_RADIX_Node_T *L_RADIX_GetNextUntil(L_RADIX_Node_T *node, L_RADIX_Node_T *limit)
{
    L_RADIX_Node_T *next;
    L_RADIX_Node_T *start;

    /* Node may be deleted from route_unlock_node so we have to preserve
    next node's pointer. */

    if (node->l_left)
    {
        next = node->l_left;
        L_RADIX_LockNode (next);
        L_RADIX_UnlockNode (node);
        return next;
    }
    if (node->l_right)
    {
        next = node->l_right;
        L_RADIX_LockNode (next);
        L_RADIX_UnlockNode (node);
        return next;
    }

    start = node;
    while (node->parent && node != limit)
    {
        if (node->parent->l_left == node && node->parent->l_right)
        {
            next = node->parent->l_right;
            L_RADIX_LockNode (next);
            L_RADIX_UnlockNode (start);
            return next;
        }
        node = node->parent;
    }
    L_RADIX_UnlockNode (start);
    return NULL;
}


/* PURPOSE: Search table by given prefix.
 * INPUT:   table, p
 * OUTPUT:  ret_node
 * RETURN:  TRUE/FALSE
 */
BOOL_T L_RADIX_LookupNode (L_RADIX_Table_T *table, 
                                L_PREFIX_T *p, 
                                L_RADIX_Node_T **ret_node)
{
    L_RADIX_Node_T *node;

    if ((NULL == table) || (NULL == p))
        return FALSE;

    node = table->top;
    /* Radix search */
    while (node && 
            node->p.prefixlen <= p->prefixlen &&
            L_PREFIX_Match (&node->p, p))
    {
        if (node->p.prefixlen == p->prefixlen)
        {
            L_RADIX_LockNode(node);
            *ret_node = node;
            return TRUE;
        }

        node = node->link[L_RADIX_CheckBit(&p->u.prefix, node->p.prefixlen)];
    }

    return FALSE;
}


/* PURPOSE: Search table by given prefix.
 * INPUT:   table, p
 * OUTPUT:  None
 * RETURN:  node if success, otherwise return NULL
 */
L_RADIX_Node_T *L_RADIX_NodeLookup (L_RADIX_Table_T *table, L_PREFIX_T *p)
{
    L_RADIX_Node_T *node;

    node = table->top;

    while (node && node->p.prefixlen <= p->prefixlen && 
            L_PREFIX_Match (&node->p, p))
    {
        if (node->p.prefixlen == p->prefixlen && node->info)
            return L_RADIX_LockNode (node);

        node = node->link[L_RADIX_CheckBit(&p->u.prefix, node->p.prefixlen)];
    }

    return NULL;
}


/* PURPOSE: Search table by given IPv4 address.
 * INPUT:   table, addr
 * OUTPUT:  None
 * RETURN:  node if success, otherwise return NULL
 */
L_RADIX_Node_T *L_RADIX_NodeLookupIPv4 (L_RADIX_Table_T *table, struct pal_in4_addr *addr)
{
    L_PREFIX_T p;

    memset (&p, 0, sizeof (L_PREFIX_T));
    p.family = AF_INET;
    p.prefixlen = IPV4_MAX_PREFIXLEN;
    p.u.prefix4 = *addr;

    return L_RADIX_NodeLookup (table, &p);
}


/* PURPOSE: Search table by given IPv6 address.
 * INPUT:   table, addr
 * OUTPUT:  None
 * RETURN:  node if success, otherwise return NULL
 */
L_RADIX_Node_T *L_RADIX_NodeLookupIPv6 (L_RADIX_Table_T *table, struct pal_in6_addr *addr)
{
#if (SYS_CPNT_INET6 == TRUE)
    L_PREFIX_T p;

    memset(&p, 0, sizeof (L_PREFIX_T));
    p.family = AF_INET6;
    p.prefixlen = IPV6_MAX_PREFIXLEN;
    p.u.prefix6 = *addr;

    return L_RADIX_NodeLookup(table, &p);
#else
    return NULL;
#endif /* SYS_CPNT_INET6 */
}


/* PURPOSE: Find matched prefix
 * INPUT:   table, p
 * OUTPUT:  None
 * RETURN:  the matched node if success, otherwise return NULL
 */
L_RADIX_Node_T *L_RADIX_NodeMatch(L_RADIX_Table_T *table, L_PREFIX_T *p)
{
    L_RADIX_Node_T *node;
    L_RADIX_Node_T *matched;

    matched = NULL;
    node = table->top;

    /* Walk down tree.  If there is matched route then store it to
     * matched. */
    while (node && node->p.prefixlen <= p->prefixlen && 
            L_PREFIX_Match (&node->p, p))
    {
        if (node->info)
            matched = node;
        node = node->link[L_RADIX_CheckBit(&p->u.prefix, node->p.prefixlen)];
    }

    /* If matched route found, return it. */
    if (matched)
        return L_RADIX_LockNode(matched);

    return NULL;
}


/* PURPOSE: Find matched node by given IPv4 address
 * INPUT:   table, p
 * OUTPUT:  None
 * RETURN:  the matched node if success, otherwise return NULL
 */
L_RADIX_Node_T *L_RADIX_NodeMatchIPv4 (L_RADIX_Table_T *table, struct pal_in4_addr *addr)
{
    L_PREFIX_T p;

    memset (&p, 0, sizeof (L_PREFIX_T));
    p.family = AF_INET;
    p.prefixlen = IPV4_MAX_PREFIXLEN;
    p.u.prefix4 = *addr;

    return L_RADIX_NodeMatch (table, &p);
}


/* PURPOSE: Find matched node by given IPv6 address
 * INPUT:   table, p
 * OUTPUT:  None
 * RETURN:  the matched node if success, otherwise return NULL
 */
L_RADIX_Node_T *L_RADIX_NodeMatchIPv6 (L_RADIX_Table_T *table, struct pal_in6_addr *addr)
{
#if (SYS_CPNT_INET6 == TRUE)
    L_PREFIX_T p;

    memset(&p, 0, sizeof (L_PREFIX_T));
    p.family = AF_INET6;
    p.prefixlen = IPV6_MAX_PREFIXLEN;
    p.u.prefix6 = *addr;

    return L_RADIX_NodeMatch(table, &p);
#else
    return NULL;
#endif /* SYS_CPNT_INET6 */
}


/* PURPOSE: Find matched prefix, excluding the node with the exclude prefix.
 * INPUT:   table, p
 * OUTPUT:  None
 * RETURN:  the matched node if success, otherwise return NULL
 */
L_RADIX_Node_T *L_RADIX_NodeMatchExclude (L_RADIX_Table_T *table, 
					     L_PREFIX_T *p,
					     L_PREFIX_T *exclude)
{
    L_RADIX_Node_T *node;
    L_RADIX_Node_T *matched;

    if (! exclude)
        return L_RADIX_NodeMatch (table, p);

    matched = NULL;
    node = table->top;

    /* Walk down tree. If there is matched route and it is not the same as
     * the exclude prefix, store it in matched. */
    while (node
            && node->p.prefixlen <= p->prefixlen
            && L_PREFIX_Match (&node->p, p))
    {
        if (node->info && ! L_PREFIX_PrefixSame (&node->p, exclude))
            matched = node;
        node = node->link[L_RADIX_CheckBit(&p->u.prefix, node->p.prefixlen)];
    }

    /* If matched route found, return it. */
    if (matched)
        return L_RADIX_LockNode (matched);

    return NULL;
}


/* PURPOSE: check if the table contains nodes with info set
 * INPUT:   table
 * OUTPUT:  None
 * RETURN:  return 1 if node->info is set, otherwise return 0
 */
u_char L_RADIX_IsTableHasInfo(L_RADIX_Table_T *table)
{
    L_RADIX_Node_T *node;

    if (table == NULL)
        return 0;

    node = table->top;

    while (node)
    {
        if (node->info)
            return 1;

        if (node->l_left)
        {
            node = node->l_left;
            continue;
        }

        if (node->l_right)
        {
            node = node->l_right;
            continue;
        }

        while (node->parent)
        {
            if (node->parent->l_left == node && node->parent->l_right)
            {
                node = node->parent->l_right;
                break;
            }
            node = node->parent;
        }

        if (node->parent == NULL)
            break;
    }

    return 0;
}


/* PURPOSE: Specify an identifier for this route table.
 * INPUT:   table, id
 * OUTPUT:  None
 * RETURN:  None
 */
void L_RADIX_SetTableId(L_RADIX_Table_T *table, u_int32_t id)
{
    if (table)
        table->id = id;
}


/* LOCAL SUBPROGRAM SPECIFICATION
 */
/* PURPOSE: Allocate a Radix node
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  the address of the allocated node and NULL on failure.
 */
static L_RADIX_Node_T *L_RADIX_NewNode(void)
{
    L_RADIX_Node_T *node;

    node = malloc(sizeof(L_RADIX_Node_T));
    if (NULL == node)
        return NULL;

    memset(node, 0, sizeof(L_RADIX_Node_T));
    return node;
}


/* PURPOSE: Free a Radix node
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  None.
 */
void L_RADIX_FreeNode(L_RADIX_Node_T *node)
{
    if (NULL == node)
        return;

    free(node);
}


/* PURPOSE: Create a Radix node with given prefix
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  the address of the created node and NULL on failure.
 */
static L_RADIX_Node_T *L_RADIX_CreateNode(L_RADIX_Table_T *table, 
                                                    struct prefix *prefix)
{
    L_RADIX_Node_T *node;

    node = L_RADIX_NewNode();
    if (NULL == node)
        return NULL;

    L_PREFIX_Copy(&node->p, prefix);
    node->table = table;

    return node;
}


/* PURPOSE: Delete a Radix node from Radix table
 * INPUT:   None
 * OUTPUT:  None
 * RETURN:  None
 */
void L_RADIX_DeleteNode(L_RADIX_Node_T *node)
{
    L_RADIX_Node_T *child;
    L_RADIX_Node_T *parent;

    assert(node->lock == 0);
    assert(node->info == NULL);

    if (node->l_left && node->l_right)
        return;

    if (node->l_left != NULL)
        child = node->l_left;
    else
        child = node->l_right;

    parent = node->parent;

    if (child != NULL)
        child->parent = parent;

    if (parent != NULL)
    {
        if (parent->l_left == node)
            parent->l_left = child;
        else
            parent->l_right = child;
    }
    else
        node->table->top = child;

    L_RADIX_FreeNode(node);

    if (parent && parent->lock == 0)
        L_RADIX_DeleteNode(parent);
}


/* PURPOSE: Dump table
 * INPUT:   t
 * OUTPUT:  None
 * RETURN:  None
 */
void L_RADIX_DumpNode(L_RADIX_Table_T *t)
{
    L_RADIX_Node_T *node;
    char buf[INET6_ADDRSTRLEN];
    
    for (node = L_RADIX_GetTableTop(t); node != NULL; node = L_RADIX_GetNext(node))
    {
        fprintf(stdout, "[%d] %p %s/%d\n", 
            node->lock, node->info,
            inet_ntop(node->p.family, &node->p.u.prefix, buf, INET6_ADDRSTRLEN),
            node->p.prefixlen);
    }
}


/* PURPOSE: Generate a common prefix
 * INPUT:   n, p
 * OUTPUT:  new_p
 * RETURN:  None
 */
static void L_RADIX_GenerateCommonPrefix(struct prefix *n, struct prefix *p,
                                    struct prefix *new_p)
{
    int i;
    UI8_T diff;
    UI8_T mask;

    UI8_T *np = (UI8_T *)&n->u.prefix;
    UI8_T *pp = (UI8_T *)&p->u.prefix;
    UI8_T *newp = (UI8_T *)&new_p->u.prefix;

    for (i = 0; i < (p->prefixlen / PNBBY); i++)
    {
        if (np[i] == pp[i])
            newp[i] = np[i];
        else
            break;
    }

    new_p->prefixlen = i * PNBBY;

    if (new_p->prefixlen != p->prefixlen)
    {
        diff = np[i] ^ pp[i];
        mask = 0x80;
        while (new_p->prefixlen < p->prefixlen && !(mask & diff))
        {
            mask >>= 1;
            new_p->prefixlen++;
        }

        newp[i] = np[i] & maskbit[new_p->prefixlen % PNBBY];
    }
}


/* PURPOSE: Check bit of the prefix.
 * INPUT:   prefix, prefixlen
 * OUTPUT:  None
 * RETURN:  Location of the prefix, 0 for left child node
 *          and 1 for right child node.
 */
static int L_RADIX_CheckBit(UI8_T *prefix, UI8_T prefixlen)
{
    int offset;
    int shift;
    UI8_T *p = (UI8_T *)prefix;

    assert(prefixlen <= IPV6_MAX_PREFIXLEN);

    offset = prefixlen / PNBBY;
    shift = 7 - (prefixlen % PNBBY);

    return (p[offset] >> shift & 1);
}


/* PURPOSE: Link two nodes.
 * INPUT:   node, new_node
 * OUTPUT:  None
 * RETURN:  None.
 */
static void L_RADIX_SetLink(L_RADIX_Node_T *node, L_RADIX_Node_T *new_node)
{
    int bit;

    bit = L_RADIX_CheckBit(&new_node->p.u.prefix, node->p.prefixlen);

    assert (bit == 0 || bit == 1);

    node->link[bit] = new_node;
    new_node->parent = node;
}

