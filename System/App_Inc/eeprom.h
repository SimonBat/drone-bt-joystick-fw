#ifndef __EEPROM_H
#define __EEPROM_H

#include "stm32f1xx_hal.h"

#define EE_PAGE_SIZE                   (uint16_t)0x400  /* Page size=1KByte */
#define EE_START_ADDRESS               ((uint32_t)0x0801F800) /* EEPROM emulation start address */
/* Pages 0 and 1 base and end addresses */
#define EE_PAGE_0_BASE_ADDRESS         ((uint32_t)(EE_START_ADDRESS+0x000))
#define EE_PAGE_0_END_ADDRESS          ((uint32_t)(EE_START_ADDRESS+(EE_PAGE_SIZE-1)))
#define EE_PAGE_1_BASE_ADDRESS         ((uint32_t)(EE_START_ADDRESS+EE_PAGE_SIZE))
#define EE_PAGE_1_END_ADDRESS          ((uint32_t)(EE_START_ADDRESS+(2*EE_PAGE_SIZE-1)))
/* Used Flash pages for EEPROM emulation */
#define EE_PAGE_0                      ((uint16_t)0x0000)
#define EE_PAGE_1                      ((uint16_t)0x0001)
/* No valid page define */
#define EE_NO_VALID_PAGE               ((uint16_t)0x00AB)
/* Page status definitions */
#define EE_ERASED                      ((uint16_t)0xFFFF) /* PAGE is empty */
#define EE_RECEIVE_DATA                ((uint16_t)0xEEEE) /* PAGE is marked to receive data */
#define EE_VALID_PAGE                  ((uint16_t)0x0000) /* PAGE containing valid data */
/* Valid pages in read and write defines */
#define EE_READ_FROM_VALID_PAGE        ((uint8_t)0x00)
#define EE_WRITE_IN_VALID_PAGE         ((uint8_t)0x01)
/* Page full define */
#define EE_PAGE_FULL                   ((uint8_t)0x80)
/* Variables number */
#define EE_VALUES_NUMBER               ((uint8_t)0x02)

/* Variables virtual address */
#define EE_SETTINGS_BATTERY_CAPACITY   ((uint16_t)0x0001)
#define EE_SETTINGS_ADC_AVG_LEVEL      ((uint16_t)0x0002)

/* Global EEPROM virtual address table */
extern uint16_t EE_VIRTUAL_ADDRESS_TABLE[EE_VALUES_NUMBER];

/* Global functions declarations */
uint16_t EE_Init(void);
uint16_t EE_Read_Variable(uint16_t _virtualAddress, uint16_t* _data);
uint16_t EE_Write_Variable(uint16_t _virtualAddress, uint16_t _data);

#endif
