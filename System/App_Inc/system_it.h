#ifndef __SYSTEM_IT_H
#define __SYSTEM_IT_H

/* Exported functions prototypes */
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void USART1_IRQHandler(void);
void USART3_IRQHandler(void);

#endif
