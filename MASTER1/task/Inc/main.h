#ifndef MAIN_H_
#define MAIN_H_

#define MAX_TASKS               4

/* some stack memory calculations */
#define SIZE_TASK_STACK         1024U
#define SIZE_SCHED_STACK        1024U

#define SRAM_START              0x20000000U
#define SIZE_SRAM               ((96)*(1024))
#define SRAM_END                (SRAM_START + SIZE_SRAM)

#define T1_STACK_START          (SRAM_END)
#define T2_STACK_START          ((SRAM_END) - (1 * SIZE_TASK_STACK))
#define T3_STACK_START          ((SRAM_END) - (2 * SIZE_TASK_STACK))
#define T4_STACK_START          ((SRAM_END) - (3 * SIZE_TASK_STACK))
#define SCHED_STACK_START       ((SRAM_END) - (4 * SIZE_TASK_STACK))

#define DUMMY_XPSR      0x01000000U


#define LED_PORT        GPIOB
#define LED1_PIN        GPIO_PIN_NO_1
#define LED2_PIN        GPIO_PIN_NO_2
#define LED3_PIN        GPIO_PIN_NO_14
#define LED4_PIN        GPIO_PIN_NO_15

#endif /* MAIN_H_ */