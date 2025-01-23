#include "stm32f401xx.h"
#include <stdio.h>
#include "main.h"
#include "led.h"

void uart2_init_pins(void);
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack);
void init_tasks_stack(void);
void enable_processor_faults(void);
void switch_sp_to_psp(void);
uint32_t get_psp_value(void);

extern int __io_putchar(int ch)
{
    uart2_write_byte((uint8_t)ch);
    return ch;

}

void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);


uint8_t current_task = 0; // task1 is running

typedef struct
{
    uint32_t psp_value;
    uint32_t block_count;
    uint8_t current_state;
    void (*task_handler)(void);
}TCB_t;

TCB_t user_tasks[MAX_TASKS];


int main(void)
 {
    uart2_init_pins();
    uart2_init();
    printf("UART INIT\n");


    enable_processor_faults();

    init_scheduler_stack(SCHED_STACK_START);
    led_init_all();
    printf("OK\n");

    init_tasks_stack();

    systick_init(TICK_HZ);

    switch_sp_to_psp();
    task1_handler();

    while (1)
    {
    }
}

void task1_handler(void)
{
    while(1)
    {
        led_on(LED1_PIN);
        delay(DELAY_COUNT_1S);
        led_off(LED1_PIN);
        delay(DELAY_COUNT_1S);
    }
}

void task2_handler(void)
{
    while(1)
    {
        led_on(LED2_PIN);
        delay(DELAY_COUNT_500MS);
        led_off(LED2_PIN);
        delay(DELAY_COUNT_500MS);
    }
}

void task3_handler(void)
{
    while(1)
    {
        led_on(LED3_PIN);
        delay(DELAY_COUNT_250MS);
        led_off(LED3_PIN);
        delay(DELAY_COUNT_250MS);
    }
}

void task4_handler(void)
{
    while(1)
    {
        led_on(LED4_PIN);
        delay(DELAY_COUNT_125MS);
        led_off(LED4_PIN);
        delay(DELAY_COUNT_125MS);
    }
}


__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack)
{
    //__asm volatile("MSR MSP, R0"); // here R0 is the first argument
    __asm volatile("MSR MSP, %0": : "r" (sched_top_of_stack): ); // other way
    __asm volatile("BX LR"); // return from function call
}


void init_tasks_stack(void)
{
    user_tasks[0].current_state = TASK_RUNNING_STATE;
    user_tasks[1].current_state = TASK_RUNNING_STATE;
    user_tasks[2].current_state = TASK_RUNNING_STATE;
    user_tasks[3].current_state = TASK_RUNNING_STATE;

    user_tasks[0].psp_value = T1_STACK_START;
    user_tasks[1].psp_value = T2_STACK_START;
    user_tasks[2].psp_value = T3_STACK_START;
    user_tasks[3].psp_value = T4_STACK_START;

    user_tasks[0].task_handler = task1_handler;
    user_tasks[1].task_handler = task2_handler;
    user_tasks[2].task_handler = task3_handler;
    user_tasks[3].task_handler = task4_handler;

    uint32_t *pPSP;

    for(int i = 0; i < MAX_TASKS; i++)
    {
        pPSP = (uint32_t*)user_tasks[i].psp_value;

        pPSP--;
        *pPSP = DUMMY_XPSR;

        pPSP--; // PC
        *pPSP = (uint32_t)user_tasks[i].task_handler;

        pPSP--; // LR
        *pPSP = 0xFFFFFFFD;

        for(int j = 0; j < 13; j++) // Rx
        {
            pPSP--;
            *pPSP = 0;
        }

        user_tasks[i].psp_value = (uint32_t)pPSP;


    }
}

uint32_t get_psp_value(void)
{
    return user_tasks[current_task].psp_value;
}

void save_psp_value(uint32_t current_psp_value)
{
    user_tasks[current_task].psp_value = current_psp_value;
}

void update_next_task(void)
{
    current_task++;
    current_task %= MAX_TASKS;
}

__attribute__((naked)) void switch_sp_to_psp(void)
{
    // 1, initialize the PSP with TASK1 stack start

    // get the value of PSP with TASK1 stack start address
    __asm volatile ("PUSH {LR}"); // preserve LR which connects back to main()
    __asm volatile ("BL get_psp_value");
    __asm volatile ("MSR PSP, R0"); // initialize psp
    __asm volatile ("POP {LR}"); // pops back LR value

    // 2. Change the SP to PSP using CONTROL register
    __asm volatile("MOV R0, #0x02");
    __asm volatile("MSR CONTROL, R0");
    __asm volatile("BX LR");

}

__attribute__((naked)) void SysTick_Handler(void)
{
    /* Save the context of the current task */

    // 1. Get current running task PSP value
    __asm volatile("MRS R0, PSP");

    // 2. Using that PSP value to store SF2 (R4 to R11)
    __asm volatile("STMDB R0!, {R4-R11}");

    __asm volatile("PUSH {LR}");
    // 3. Save the current value of PSP
    __asm volatile("BL save_psp_value");


    /* Retrieve the context of the next task */

    // 1. Decide next task to run
    __asm volatile ("BL update_next_task");

    // 2. Get its past PSP value
    __asm volatile ("BL get_psp_value");

    // 3. Using that PSP value retrieve SF2 (R4 to R11) 
    __asm volatile ("LDMIA R0!, {R4-R11}");

    // 4. Update PSP and exit
    __asm volatile ("MSR PSP, R0");

    __asm volatile("POP {LR}");
    __asm volatile("BX LR");
}

void enable_processor_faults(void)
{
    uint32_t *pSHCSR = (uint32_t*)0xE000ED24;

    *pSHCSR |= (1 << 16); // mem manage
    *pSHCSR |= (1 << 17); // bus fault
    *pSHCSR |= (1 << 18); // usage fault
}


// implementation the fault handlers
void HardFault_Handler(void)
{
    printf("Exception : Hardfault\n");
    while(1);
}

void MemManage_Handler(void)
{
    printf("Exception: MemManage\n");
    while(1);
}


void BusFault_Handler(void)
{
    printf("Exception: BusFault\n");
    while(1);
}
