/*
 *  sysfun_utest.c
 *  cli
 *
 *  Created by yehjunying on 2010/8/29.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

/* This file should be one part of the sysfun.c
 */

#include "sysfun_utest.h"

#define LOG(fmt, args...)                       \
{                                               \
    printf("%s:%d ", __FUNCTION__, __LINE__);   \
    printf(fmt, ##args);                        \
    printf("\n");                               \
    fflush(stdout);                             \
}

static BOOL_T SYSFUN_UTEST_UART()
{
    UI32_T uart_handler = SYSFUN_RESULT_ERROR;
    BOOL_T ret = TRUE;
    
    uart_handler = SYSFUN_OpenUART(SYSFUN_UART_CHANNEL1);
    
    if (SYSFUN_RESULT_ERROR == uart_handler)
    {
        LOG("SYSFUN_OpenUART failed");
        return FALSE;
    }
    
    
    {
        char c;
        UI32_T len;

        LOG("Input any key(\'x\' to exit) to test UART function.");
        
        while (TRUE)
        {
            len = SYSFUN_UARTPollRxBuff(uart_handler, 1, &c);

            if (1 == len)
            {
                if (c == 'x' || c == 'X')
                {
                    break;
                }

                SYSFUN_UARTPutData(uart_handler, 1, &c);
            }
        }
    }
    
    if (SYSFUN_RESULT_ERROR != uart_handler)
    {
        SYSFUN_CloseUART(uart_handler);
    }
    
    return ret;
}

#include <sys/times.h> /* for SYSFUN_UTEST_Sleep */

static BOOL_T SYSFUN_UTEST_Sleep()
{
    BOOL_T ret = TRUE;
    clock_t cur_clk;
    struct tms buf;
    
    LOG("Test SYSFUN_Sleep. Process will sleep abort 3 second.");
    
    cur_clk = times(&buf);
    LOG("Current second = %lu", cur_clk);
    
    SYSFUN_Sleep(300);
    
    cur_clk = times(&buf);
    LOG("Current second = %lu", cur_clk);
    
    return ret;
}

static BOOL_T SYSFUN_UTEST_Timer_Thread()
{
#define SYSFUN_UTEST_TIMER_EVENT    1
    
#define SYSFUN_UTEST_INTERVAL_TICK  200
#define SYSFUN_UTEST_MAX_WAIT_EVENT 2
    
    void * tid = SYSFUN_PeriodicTimer_Create();
    int count = 0;
    
    SYSFUN_PeriodicTimer_Start(tid,
                               SYSFUN_UTEST_INTERVAL_TICK,
                               SYSFUN_UTEST_TIMER_EVENT);
    
    while(1)
    {
        UI32_T received_events;
        
        SYSFUN_ReceiveEvent (SYSFUN_UTEST_TIMER_EVENT,
                             SYSFUN_EVENT_WAIT_ANY,
                             SYSFUN_TIMEOUT_WAIT_FOREVER,
                             &received_events);
        if (received_events | SYSFUN_UTEST_TIMER_EVENT)
        {
            LOG("Receive timer event %lu\r\n", SYSFUN_GetSysTick());
            
            if (SYSFUN_UTEST_MAX_WAIT_EVENT == count++)
            {
                break;
            }
        }
    }
    
    SYSFUN_PeriodicTimer_Destroy(tid);
    
    return TRUE;
}

static BOOL_T SYSFUN_UTEST_Timer()
{
    void   *thread_obj;
    UI32_T  thread_id;
    UI32_T  ret;
    
    ret = SYSFUN_SpawnJoinableThread(SYS_BLD_L4_GROUP_MGR_THREAD_PRIORITY,
                                     SYS_BLD_L4_GROUP_MGR_SCHED_POLICY,
                                     SYS_BLD_L4_GROUP_MGR_THREAD_NAME,
                                     SYS_BLD_TASK_COMMON_STACK_SIZE,
                                     SYSFUN_TASK_FP,
                                     SYSFUN_UTEST_Timer_Thread,
                                     NULL,
                                     &thread_id,
                                     &thread_obj);
    if (ret != SYSFUN_OK)
    {
        LOG("SYSFUN_SpawnJoinableThread");
        return FALSE;
    }
    
    ret = SYSFUN_JoinThread(&thread_obj);
    if (ret != SYSFUN_OK)
    {
        LOG("SYSFUN_JoinThread");
        return FALSE;
    }
    
    return TRUE;
}
    
BOOL_T SYSFUN_UTEST_Main(int argc, const char * argv[])
{
    BOOL_T ret = TRUE;
    
    SYSFUN_Init();
    
    ret &= SYSFUN_UTEST_UART();
    ret &= SYSFUN_UTEST_Sleep();
    ret &= SYSFUN_UTEST_Timer();
    return ret;
}

