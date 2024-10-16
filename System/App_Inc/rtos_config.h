#ifndef __RTOS_CONFIG_H
#define __RTOS_CONFIG_H

extern uint32_t SystemCoreClock;

#define configUSE_PREEMPTION				                1U
#define configUSE_IDLE_HOOK					                0U
#define configUSE_TICK_HOOK					                0U
#define configCPU_CLOCK_HZ					                SystemCoreClock
#define configTICK_RATE_HZ					                ((TickType_t)1000U)
#define configMAX_PRIORITIES				                5U
#define configMINIMAL_STACK_SIZE			                ((uint16_t)80U)
#define configTOTAL_HEAP_SIZE				                ((size_t)(800U))
#define configMAX_TASK_NAME_LEN				                64U
#define configUSE_TRACE_FACILITY			                1U
#define configUSE_16_BIT_TICKS				                0U
#define configIDLE_SHOULD_YIELD				                1U
#define configUSE_MUTEXES					                1U

#define configQUEUE_REGISTRY_SIZE			                8U
#define configCHECK_FOR_STACK_OVERFLOW		                0U
#define configUSE_RECURSIVE_MUTEXES			                1U
#define configUSE_MALLOC_FAILED_HOOK		                0U
#define configUSE_APPLICATION_TASK_TAG		                0U
#define configUSE_COUNTING_SEMAPHORES		                1U
#define configGENERATE_RUN_TIME_STATS		                0U
#define configSUPPORT_DYNAMIC_ALLOCATION	                1U
#define configSUPPORT_STATIC_ALLOCATION 					1U
#define configSTACK_DEPTH_TYPE                              uint16_t

/* Co-routine definitions */
#define configUSE_CO_ROUTINES 				                0U
#define configMAX_CO_ROUTINE_PRIORITIES 	                2U

/* Software timer definitions */
#define configUSE_TIMERS					                1U
#define configTIMER_TASK_PRIORITY			                3U
#define configTIMER_QUEUE_LENGTH			                100U
#define configTIMER_TASK_STACK_DEPTH		                (configMINIMAL_STACK_SIZE)

/* Set the following definitions to 1 to include the API function, or zero to exclude the API function */
#define INCLUDE_vTaskPrioritySet			                1U
#define INCLUDE_uxTaskPriorityGet			                1U
#define INCLUDE_vTaskDelete					                1U
#define INCLUDE_vTaskCleanUpResources		                1U
#define INCLUDE_vTaskSuspend				                1U
#define INCLUDE_vTaskDelayUntil				                1U
#define INCLUDE_vTaskDelay					                1U

/* Cortex-M specific definitions */
#define configPRIO_BITS       				                4U /* 15 priority levels */

/* The lowest interrupt priority that can be used in a call to a "set priority" function */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			    0x0f

/** The highest interrupt priority that can be used by any interrupt service
  * routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
  * INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
  * PRIORITY THAN THIS! (higher priorities are lower numeric values.
  */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	    3U

/** Interrupt priorities used by the kernel port layer itself.  These are generic
  * to all Cortex-M ports, and do not rely on any particular library functions
  */
#define configKERNEL_INTERRUPT_PRIORITY 		            (configLIBRARY_LOWEST_INTERRUPT_PRIORITY<<(8-configPRIO_BITS))
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!! */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	            (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY<<(8-configPRIO_BITS))
	
/* Normal assert() semantics without relying on the provision of an assert.h header file */
#define configASSERT(x)				 						if(0==(x)){taskDISABLE_INTERRUPTS();for(;;);}
	
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS standard names */
#define vPortSVCHandler 									SVC_Handler
#define xPortPendSVHandler 									PendSV_Handler
#define xPortSysTickHandler 								SysTick_Handler

#endif
