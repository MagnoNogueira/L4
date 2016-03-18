#ifndef _UTEST_H_
#define _UTEST_H_

#define UT_MAX_DESC_STR_LEN         64
#define UT_MAX_SHELL_ITEM_STR_LEN   10

//#define UT_FAIL(x)                  ( RULE_TYPE_OK != (x) )
//#define UT_SUCCESS(x)               ( !(UT_FAIL(x)) )

#define UT_REGISTER(setup, setdown, test)               \
    { UT_Register(setup, setdown, test, #test); }

#if (1400 <= _MSC_VER /* VC2005 */)
#define UT_LOG(fmt, ...)                                \
    {                                                   \
        printf("[%s:%d] ", __FUNCTION__, __LINE__);     \
        printf(fmt, __VA_ARGS__);                       \
        printf("\r\n");                                 \
    }
#else
#define UT_LOG(fmt, args...)                            \
    {                                                   \
        printf("[%s:%d] ", __FUNCTION__, __LINE__);     \
        printf(fmt, ##args);                            \
        printf("\r\n");                                 \
    }
#endif /* _MSC_VER */

enum
{
    UT_OK   = 0,
    UT_FAIL = -1
};

typedef int (*UT_TestFunction) ();
typedef void (*UI_ShellItemFunction) ();

typedef struct UT_TestItem
{
    UT_TestFunction     setup;
    UT_TestFunction     run;
    UT_TestFunction     setdown;
    char                desc[UT_MAX_DESC_STR_LEN + 1];

    struct UT_TestItemFlag
    {
        int             isRun;
        int             isPass;
    } flag;

    struct UT_TestItem  *next;

} UT_TestItem;

typedef struct UT_ShellItem
{
    char                    name[UT_MAX_SHELL_ITEM_STR_LEN + 1];
    UI_ShellItemFunction    func;
    struct UT_ShellItem     *next;
} UT_ShellItem;


void UT_Init();

void UT_Register(
    UT_TestFunction setup,
    UT_TestFunction setdown,
    UT_TestFunction run,
    char *desc
    );

int UT_RegisterShellItem(
    UT_ShellItem *si
    );

int UT_Run();
int UT_Shell();


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
            UT_TestFunction run);

#endif /* _UTEST_H_ */

