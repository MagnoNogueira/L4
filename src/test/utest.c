#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // tolower

#include "utest.h"

static UT_TestItem  *utest_test_itmes;
static UT_ShellItem *utest_shell_items;

static UT_TestItem* ut_get_test_item_ptr()
{
    return utest_test_itmes;
}

static UT_ShellItem* ut_get_shell_item_ptr()
{
    return utest_shell_items;
}

static void ut_set_shell_item_ptr(UT_ShellItem *pSi)
{
    utest_shell_items = pSi;
}

static int ut_get_number_of_total_test_item()
{
    UT_TestItem *pItem = ut_get_test_item_ptr();
    int num = 0;

    while (pItem)
    {
        num ++;
        pItem = pItem->next;
    }

    return num;
}

static int ut_get_number_of_untest_item()
{
    UT_TestItem *pItem = ut_get_test_item_ptr();
    int num = 0;

    while (pItem)
    {
        if (pItem->flag.isRun == 0)
        {
            num ++;
        }
        pItem = pItem->next;
    }

    return num;
}

static int ut_run(int step)
{
    UT_TestItem  *func_entry;
    int          res = UT_OK;

    if (0 == step)
    {
        return 0;
    }

    func_entry = utest_test_itmes;

    while (func_entry)
    {
        if (0 == func_entry->flag.isRun)
        {
            int _res;

            if (func_entry->desc[0])
            {
                printf("Run Test: %s\n", func_entry->desc);
            }

            if (func_entry->setup)
                func_entry->setup();

            _res = func_entry->run();
            func_entry->flag.isRun = 1;
            func_entry->flag.isPass = (_res == 0) ? 1 : 0;

            res += _res;

            if (func_entry->setdown)
                func_entry->setdown();

            if (0 == --step)
            {
                break;
            }
        }

        func_entry = func_entry->next;
    }

    return res;
}

void UT_Init()
{
    UT_TestItem     *ti;

    ti = utest_test_itmes;

    while (ti)
    {
        UT_TestItem *temp = ti;
        ti = ti->next;
        free(temp);
    }

    utest_test_itmes = NULL;
    utest_shell_items = NULL;
}

void UT_Register(
    UT_TestFunction setup,
    UT_TestFunction setdown,
    UT_TestFunction run,
    char *desc)
{
    UT_TestItem   *func_entry;
    UT_TestItem   *tail_entry;

    func_entry = calloc(1, sizeof(*func_entry));
    if (func_entry == NULL)
    {
        printf("%s: out of memory\r\n", __FUNCTION__);
        return;
    }

    func_entry->setup       = setup;
    func_entry->run         = run;
    func_entry->setdown     = setdown;

    if (desc)
    {
        strncpy(func_entry->desc, desc, sizeof(func_entry->desc)-1);
        func_entry->desc[ sizeof(func_entry->desc)-1 ] = '\0';
    }

    func_entry->next        = NULL;

    if (utest_test_itmes == NULL)
    {
        utest_test_itmes = func_entry;
    }
    else
    {
        tail_entry = utest_test_itmes;

        while (tail_entry)
        {
            if (tail_entry->next == NULL)
            {
                tail_entry->next = func_entry;
                break;
            }
            tail_entry = tail_entry->next;
        }
    }
}

int UT_RegisterShellItem(
    UT_ShellItem *pSi
    )
{
    UT_ShellItem *temp = ut_get_shell_item_ptr();
    UT_ShellItem *new_item = NULL;

    while (temp)
    {
        if (0 == strcmp(pSi->name, temp->name))
        {
            /* Name have be registered
             */
            return UT_OK;
        }
        temp = temp->next;
    }

    new_item = calloc(sizeof(*new_item), 1);
    if (NULL == new_item)
    {
        printf("%s: out of memory\r\n", __FUNCTION__);
        return UT_FAIL;
    }

    *new_item = *pSi;
    new_item->next = NULL;

    temp = ut_get_shell_item_ptr();
    if (NULL == temp)
    {
        ut_set_shell_item_ptr(new_item);
    }
    else
    {
        /* goto tail node
         */
        while (temp->next)
        {
            temp = temp->next;
        }

        temp->next = new_item;
    }

    return UT_OK;
}

int UT_ShowShellItem()
{
    UT_ShellItem *pSi = ut_get_shell_item_ptr();
    int no = 0;

    while (pSi)
    {
        printf("(%d)%s\n", no++, pSi->name);

        pSi = pSi->next;
    }

    return UT_OK;
}

int UT_RunShellItem(int runNo)
{
    UT_ShellItem *pSi = ut_get_shell_item_ptr();
    int no = 0;
    int res = UT_OK;

    while (pSi)
    {
        //printf("(%d) %s\n", no++, pSi->name);
        if (no++ == runNo)
        {
            pSi->func();
            res = UT_OK;
            break;
        }

        pSi = pSi->next;
    }

    return res;
}

int UT_Run()
{
    int          res = UT_OK;
    int          num = ut_get_number_of_untest_item();

    printf("Start Test ...\n");

    res = ut_run(num);

    printf("Finish Test: %s\n", (res) ? "FAIL" : "SUCCEED");

    return res;
}

int UT_Step()
{
    int          res = UT_OK;
    int          num = ut_get_number_of_untest_item();

    if (num == 0)
    {
        printf("No Test Item\n");
        return 0;
    }

    printf("Start Test ...\n");

    res = ut_run(1);

    printf("Finish Test: %s\n", (res) ? "FAIL" : "SUCCEED");

    return res;
}

int UT_Restart()
{
    UT_TestItem *pItem = ut_get_test_item_ptr();

    while (pItem)
    {
        memset(&pItem->flag, 0, sizeof(pItem->flag));
        pItem = pItem->next;
    }

    return UT_OK;
}

int UT_List()
{
    UT_TestItem *pItem = ut_get_test_item_ptr();

    while (pItem)
    {
        printf(" %s, isRun=%d, isPass=%s\r\n",
               pItem->desc,
               pItem->flag.isRun,
               pItem->flag.isRun?(pItem->flag.isPass?"Yes":"No"):"NA"
               );
        pItem = pItem->next;
    }

    return UT_OK;
}

int UT_Shell()
{
    typedef enum
    {
        CONTINUE,
        STOP
    } STATUS;

    int chChoice;
    char szTemp[256];
    //CU_pSuite pSuite = NULL;
    STATUS eStatus = CONTINUE;

    while (CONTINUE == eStatus)
    {
        fprintf(stdout, "\n******************* UNIT TEST - MAIN MENU ***********************"
                        "\n(R)un all, Run (S)ingle, Restar(t), (L)ist all, (Q)uit"
                        "\n"
                        /*"\nEnter Command : "*/);

        UT_ShowShellItem();

        fprintf(stdout, "\nEnter Command : ");

        chChoice = getchar();
        fgets(szTemp, sizeof(szTemp), stdin);

        switch (tolower(chChoice))
        {
        case 'r':
            UT_Run();
            break;

        case 's':
            UT_Step();
            break;

        case 't':
            UT_Restart();
            break;

        case 'l':
            UT_List();
            break;

        case 'q':
            eStatus = STOP;
            break;

        default:
            if ('0' <= chChoice && chChoice <= '9')
            {
                UT_RunShellItem(chChoice-'0');
            }
            break;
        }
    }

    return UT_OK;
}

/* -------------------------------------------------------------------------
 * Function : UT_Test
 * -------------------------------------------------------------------------
 * Purpose  : Run a test item
 * INPUT    : setup		- Setup function
 *			  teardown	- Teardown function
 *			  run		- Run function
 * OUTPUT   : None
 * RETURN   : UT_OK, or error code
 * NOTE     : None
 * -------------------------------------------------------------------------
 */
int UT_Test(UT_TestFunction setup,
            UT_TestFunction teardown,
            UT_TestFunction run)
{
    int result;

    if (setup)
        setup();

    result = run();

    if (teardown)
        teardown();

    return result;
}
