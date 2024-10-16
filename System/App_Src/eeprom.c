/**
  ***************************************************************************************************************************************
  * @file     eeprom.c
  * @owner    SimonBat
  * @version  v0.0.1
  * @date     2021.11.11
  * @update   2021.11.11
  * @brief    dragonfly_joystick v1.0
  ***************************************************************************************************************************************
  * @attention
  *
  * (Where to use)
  *
  ***************************************************************************************************************************************
  */

#include "eeprom.h"

uint16_t EE_DATA_VARIABLE;
uint16_t EE_VIRTUAL_ADDRESS_TABLE[EE_VALUES_NUMBER]={EE_SETTINGS_BATTERY_CAPACITY};

/* Local functions declarations */
static HAL_StatusTypeDef EE_Format(void);
static uint16_t EE_Find_Valid_Page(uint8_t _operation);
static uint16_t EE_Verify_Page_Full_Write_Variable(uint16_t _virtualAddress, uint16_t _data);
static uint16_t EE_Page_Transfer(uint16_t _virtualAddress, uint16_t _data);
static HAL_StatusTypeDef EE_FLASH_Page_Erase(uint32_t _pageAddress);
static HAL_StatusTypeDef EE_FLASH_Write_Half_Word(uint32_t _address, uint16_t _data);

/**
  ***************************************************************************************************************************************
  * @brief  Restore the pages to a known good state in case of page's status
  *         corruption after a power loss.
  * @param  None.
  * @retval - Flash error code: on write Flash error
  *         - FLASH_COMPLETE: on success
  ***************************************************************************************************************************************
  */
uint16_t EE_Init(void)
{
    int16_t _x=-1;
    uint16_t _varIdx=0U;
    uint16_t _flashStatus;
    uint16_t _eepromStatus=0U, _readStatus=0U;
    uint16_t _pageStatus_0=6U, _pageStatus_1=6U;

    /* Get Page0 status */
    _pageStatus_0=(*(__IO uint16_t*)EE_PAGE_0_BASE_ADDRESS);
    /* Get Page1 status */
    _pageStatus_1=(*(__IO uint16_t*)EE_PAGE_1_BASE_ADDRESS);

    /* Check for invalid header states and repair if necessary */
    switch(_pageStatus_0)
    {
        case EE_ERASED:
        	if(EE_VALID_PAGE==_pageStatus_1) /* Page0 erased, Page1 valid */
        	{
        		/* Erase Page0 */
        		_flashStatus=EE_FLASH_Page_Erase(EE_PAGE_0_BASE_ADDRESS);
        		/* If erase operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        	}
        	else if(EE_RECEIVE_DATA==_pageStatus_1) /* Page0 erased, Page1 receive */
        	{
        		/* Erase Page0 */
        		_flashStatus=EE_FLASH_Page_Erase(EE_PAGE_0_BASE_ADDRESS);
        		/* If erase operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        		/* Mark Page1 as valid */
        		_flashStatus=EE_FLASH_Write_Half_Word(EE_PAGE_1_BASE_ADDRESS, EE_VALID_PAGE);
        		/* If program operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        	}
        	else /* First EEPROM access (Page0&1 are erased) or invalid state -> format EEPROM */
        	{
        		/* Erase both Page0 and Page1 and set Page0 as valid page */
        		_flashStatus=EE_Format();
        		/* If erase/program operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        	}
        break;

        case EE_RECEIVE_DATA:
        	if(EE_VALID_PAGE==_pageStatus_1) /* Page0 receive, Page1 valid */
        	{
        		/* Transfer data from Page1 to Page0 */
        		for(_varIdx=0U;_varIdx<EE_VALUES_NUMBER;_varIdx++)
        		{
        			if((*(__IO uint16_t*)(EE_PAGE_0_BASE_ADDRESS+6U))==EE_VIRTUAL_ADDRESS_TABLE[_varIdx]){_x=_varIdx;}
        			if(_varIdx!=_x)
        			{
        				/* Read the last variables' updates */
        				_readStatus=EE_Read_Variable(EE_VIRTUAL_ADDRESS_TABLE[_varIdx],&EE_DATA_VARIABLE);
        				/* In case variable corresponding to the virtual address was found */
        				if(0x01!=_readStatus)
        				{
        					/* Transfer the variable to the Page0 */
        					_eepromStatus=EE_Verify_Page_Full_Write_Variable(EE_VIRTUAL_ADDRESS_TABLE[_varIdx], EE_DATA_VARIABLE);
        					/* If program operation was failed, a Flash error code is returned */
        					if(HAL_OK!=_eepromStatus){return _eepromStatus;}
        				}
        			}
        		}
        
        		/* Mark Page0 as valid */
        		_flashStatus=EE_FLASH_Write_Half_Word(EE_PAGE_0_BASE_ADDRESS, EE_VALID_PAGE);
        		/* If program operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        		/* Erase Page1 */
        		_flashStatus=EE_FLASH_Page_Erase(EE_PAGE_1_BASE_ADDRESS);
        		/* If erase operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        	}
        	else if(EE_ERASED==_pageStatus_1) /* Page0 receive, Page1 erased */
        	{
        		/* Erase Page1 */
        		_flashStatus=EE_FLASH_Page_Erase(EE_PAGE_1_BASE_ADDRESS);
        		/* If erase operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        		/* Mark Page0 as valid */
        		_flashStatus=EE_FLASH_Write_Half_Word(EE_PAGE_0_BASE_ADDRESS, EE_VALID_PAGE);
        		/* If program operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        	}
        	else /* Invalid state -> format eeprom */
        	{
        		/* Erase both Page0 and Page1 and set Page0 as valid page */
        		_flashStatus=EE_Format();
        		/* If erase/program operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        	}
        break;

        case EE_VALID_PAGE:
        	if(EE_VALID_PAGE==_pageStatus_1) /* Invalid state -> format eeprom */
        	{
        		/* Erase both Page0 and Page1 and set Page0 as valid page */
        		_flashStatus=EE_Format();
        		/* If erase/program operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        	}
        	else if(EE_ERASED==_pageStatus_1) /* Page0 valid, Page1 erased */
        	{
        		/* Erase Page1 */
        		_flashStatus=EE_FLASH_Page_Erase(EE_PAGE_1_BASE_ADDRESS);
        		/* If erase operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        	}
        	else /* Page0 valid, Page1 receive */
        	{
        		/* Transfer data from Page0 to Page1 */
        		for(_varIdx=0U;_varIdx<EE_VALUES_NUMBER;_varIdx++)
        		{
        			if((*(__IO uint16_t*)(EE_PAGE_1_BASE_ADDRESS+6U))==EE_VIRTUAL_ADDRESS_TABLE[_varIdx]){_x=_varIdx;}
        			if(_varIdx!=_x)
        			{
        				/* Read the last variables' updates */
        				_readStatus=EE_Read_Variable(EE_VIRTUAL_ADDRESS_TABLE[_varIdx],&EE_DATA_VARIABLE);
        				/* In case variable corresponding to the virtual address was found */
        				if(0x01!=_readStatus)
        				{
        					/* Transfer the variable to the Page1 */
        					_eepromStatus=EE_Verify_Page_Full_Write_Variable(EE_VIRTUAL_ADDRESS_TABLE[_varIdx],EE_DATA_VARIABLE);
        					/* If program operation was failed, a Flash error code is returned */
        					if(HAL_OK!=_eepromStatus){return _eepromStatus;}
        				}
        			}
        		}
        
        		/* Mark Page1 as valid */
        		_flashStatus=EE_FLASH_Write_Half_Word(EE_PAGE_1_BASE_ADDRESS, EE_VALID_PAGE);
        		/* If program operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        		/* Erase Page0 */
        		_flashStatus=EE_FLASH_Page_Erase(EE_PAGE_0_BASE_ADDRESS);
        		/* If erase operation was failed, a Flash error code is returned */
        		if(HAL_OK!=_flashStatus){return _flashStatus;}
        	}
        break;

        default:  /* Any other state -> format eeprom */
        	/* Erase both Page0 and Page1 and set Page0 as valid page */
        	_flashStatus=EE_Format();
        	/* If erase/program operation was failed, a Flash error code is returned */
        	if(HAL_OK!=_flashStatus){return _flashStatus;}
        break;
    }

   return HAL_OK;
}

/**
  ***************************************************************************************************************************************
  * @brief  Returns the last stored variable data, if found, which correspond to
  *         the passed virtual address
  * @param  VirtAddress: Variable virtual address
  * @param  Data: Global variable contains the read variable value
  * @retval Success or error status:
  *           - 0: if variable was found
  *           - 1: if the variable was not found
  *           - NO_VALID_PAGE: if no valid page was found.
  ***************************************************************************************************************************************
  */
uint16_t EE_Read_Variable(uint16_t _virtualAddress, uint16_t* _data)
{
    uint16_t _readStatus=1U;
    uint16_t _validPage=EE_PAGE_0;
    uint32_t _address=EE_START_ADDRESS, _pageStartAddress=EE_START_ADDRESS;

    /* Get active Page for read operation */
    _validPage=EE_Find_Valid_Page(EE_READ_FROM_VALID_PAGE);

    /* Check if there is no valid page */
    if(EE_NO_VALID_PAGE==_validPage){return  EE_NO_VALID_PAGE;}

    /* Get the valid Page start Address */
    _pageStartAddress=(uint32_t)(EE_START_ADDRESS+(uint32_t)(_validPage*EE_PAGE_SIZE));
    /* Get the valid Page end Address */
    _address=(uint32_t)((EE_START_ADDRESS-2U)+(uint32_t)((1U+_validPage)*EE_PAGE_SIZE));

    /* Check each active page address starting from end */
    while(_address>(_pageStartAddress+2U))
    {
        /* Get the current location content to be compared with virtual address */
    	uint16_t _addressValue=(*(__IO uint16_t*)_address);

        /* Compare the read address with the virtual address */
        if(_addressValue==_virtualAddress)
        {
            /* Get content of Address-2 which is variable value */
            *_data=(*(__IO uint16_t*)(_address-2U));
            /* In case variable value is read, reset ReadStatus flag */
            _readStatus=0U;
            break;
        }
        else
        {
            /* Next address location */
            _address=_address-4U;
        }
    }

    /* Return ReadStatus value: (0: variable exist, 1: variable doesn't exist) */
    return _readStatus;
}

/**
  ***************************************************************************************************************************************
  * @brief  Writes/upadtes variable data in EEPROM.
  * @param  VirtAddress: Variable virtual address
  * @param  Data: 16 bit data to be written
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  ***************************************************************************************************************************************
  */
uint16_t EE_Write_Variable(uint16_t _virtualAddress, uint16_t _data)
{
    /* Write the variable virtual address and value in the EEPROM */
    uint16_t _status=EE_Verify_Page_Full_Write_Variable(_virtualAddress,_data);

    /* In case the EEPROM active page is full */
    if(EE_PAGE_FULL==_status)
    {
        /* Perform Page transfer */
        _status=EE_Page_Transfer(_virtualAddress,_data);
    }

    /* Return last operation status */
    return _status;
}

/**
  ***************************************************************************************************************************************
  * @brief  Erases PAGE0 and PAGE1 and writes VALID_PAGE header to PAGE0
  * @param  None
  * @retval Status of the last operation (Flash write or erase) done during
  *         EEPROM formating
  ***************************************************************************************************************************************
  */
static HAL_StatusTypeDef EE_Format(void)
{
    HAL_StatusTypeDef _flashStatus=HAL_OK;

    /* Erase Page0 */
    _flashStatus=EE_FLASH_Page_Erase(EE_PAGE_0_BASE_ADDRESS);

    /* If erase operation was failed, a Flash error code is returned */
    if(HAL_OK!=_flashStatus){return _flashStatus;}

    /* Set Page0 as valid page: Write VALID_PAGE at Page0 base address */
    _flashStatus=EE_FLASH_Write_Half_Word(EE_PAGE_0_BASE_ADDRESS,EE_VALID_PAGE);

    /* If program operation was failed, a Flash error code is returned */
    if(HAL_OK!=_flashStatus){return _flashStatus;}

    /* Erase Page1 */
    _flashStatus=EE_FLASH_Page_Erase(EE_PAGE_1_BASE_ADDRESS);

    /* Return Page1 erase operation status */
    return _flashStatus;
}

/**
  ***************************************************************************************************************************************
  * @brief  Find valid Page for write or read operation
  * @param  Operation: operation to achieve on the valid page.
  *         This parameter can be one of the following values:
  *         @arg READ_FROM_VALID_PAGE: read operation from valid page
  *         @arg WRITE_IN_VALID_PAGE: write operation from valid page
  * @retval Valid page number (PAGE0 or PAGE1) or NO_VALID_PAGE in case
  *         of no valid page was found
  ***************************************************************************************************************************************
  */
static uint16_t EE_Find_Valid_Page(uint8_t _operation)
{
    uint16_t _pageStatus_0=6U, _pageStatus_1=6U;

    /* Get Page0 actual status */
    _pageStatus_0=(*(__IO uint16_t*)EE_PAGE_0_BASE_ADDRESS);
    /* Get Page1 actual status */
    _pageStatus_1=(*(__IO uint16_t*)EE_PAGE_1_BASE_ADDRESS);

    /* Write or read operation */
    switch(_operation)
    {
        case EE_WRITE_IN_VALID_PAGE:
        	if(EE_VALID_PAGE==_pageStatus_1)
        	{
        		/* Page0 receiving data */
        		if(EE_RECEIVE_DATA==_pageStatus_0){return EE_PAGE_0;}
        		else{return EE_PAGE_1;}
        	}
        	else if(EE_VALID_PAGE==_pageStatus_0)
        	{
        		/* Page1 receiving data */
        		if(EE_RECEIVE_DATA==_pageStatus_1){return EE_PAGE_1;}
        		else{return EE_PAGE_0;}
        	}
        	else{return EE_NO_VALID_PAGE;}

        case EE_READ_FROM_VALID_PAGE:
        	if(EE_VALID_PAGE==_pageStatus_0){return EE_PAGE_0;}
        	else if(EE_VALID_PAGE==_pageStatus_1){return EE_PAGE_1;}
        	else{return EE_NO_VALID_PAGE;}

        default: return EE_PAGE_0;
    }
}

/**
  ***************************************************************************************************************************************
  * @brief  Verify if active page is full and Writes variable in EEPROM.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *          - FLASH_COMPLETE: on success
  *          - PAGE_FULL: if valid page is full
  *          - NO_VALID_PAGE: if no valid page was found
  *          - Flash error code: on write Flash error
  ***************************************************************************************************************************************
  */
static uint16_t EE_Verify_Page_Full_Write_Variable(uint16_t _virtualAddress, uint16_t _data)
{
	HAL_StatusTypeDef _flashStatus;
    uint16_t _validPage=EE_PAGE_0;
    uint32_t _address=EE_START_ADDRESS, _pageEndAddress=EE_PAGE_0_END_ADDRESS;

    /* Get valid Page for write operation */
    _validPage=EE_Find_Valid_Page(EE_WRITE_IN_VALID_PAGE);

    /* Check if there is no valid page */
    if(EE_NO_VALID_PAGE==_validPage){return  EE_NO_VALID_PAGE;}

    /* Get the valid Page start Address */
    _address=(uint32_t)(EE_START_ADDRESS+(uint32_t)(_validPage*EE_PAGE_SIZE));
    /* Get the valid Page end Address */
    _pageEndAddress=(uint32_t)((EE_START_ADDRESS-2U)+(uint32_t)((1U+_validPage)*EE_PAGE_SIZE));

    /* Check each active page address starting from begining */
    while(_address<_pageEndAddress)
    {
        /* Verify if Address and Address+2 contents are 0xFFFFFFFF */
        if(0xFFFFFFFF==(*(__IO uint32_t*)_address))
        {
            /* Set variable data */
            _flashStatus=EE_FLASH_Write_Half_Word(_address, _data);
            /* If program operation was failed, a Flash error code is returned */
            if(HAL_OK!=_flashStatus){return _flashStatus;}
            /* Set variable virtual address */
            _flashStatus=EE_FLASH_Write_Half_Word(_address+2U,_virtualAddress);

            /* Return program operation status */
            return _flashStatus;
        }
        else{_address=_address+4U;}
    }

    /* Return PAGE_FULL in case the valid page is full */
    return EE_PAGE_FULL;
}

/**
  ***************************************************************************************************************************************
  * @brief  Transfers last updated variables data from the full Page to
  *         an empty one.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *          - FLASH_COMPLETE: on success
  *          - PAGE_FULL: if valid page is full
  *          - NO_VALID_PAGE: if no valid page was found
  *          - Flash error code: on write Flash error
  ***************************************************************************************************************************************
  */
static uint16_t EE_Page_Transfer(uint16_t _virtualAddress, uint16_t _data)
{
    HAL_StatusTypeDef _flashStatus=HAL_OK;
    uint16_t _eepromStatus=0U;
    uint16_t _validPage=EE_PAGE_0;
    uint32_t _newPageAddress=EE_PAGE_0_END_ADDRESS, _oldPageAddress=EE_START_ADDRESS;
  
    /* Get active Page for read operation */
    _validPage=EE_Find_Valid_Page(EE_READ_FROM_VALID_PAGE);

    if(EE_PAGE_1==_validPage) /* Page1 valid */
    {
        /* New page address where variable will be moved to */
        _newPageAddress=EE_PAGE_0_BASE_ADDRESS;

        /* Old page address where variable will be taken from */
        _oldPageAddress=EE_PAGE_1_BASE_ADDRESS;
    }
    else if(EE_PAGE_0==_validPage) /* Page0 valid */
    {
        /* New page address where variable will be moved to */
        _newPageAddress=EE_PAGE_1_BASE_ADDRESS;

        /* Old page address where variable will be taken from */
        _oldPageAddress=EE_PAGE_0_BASE_ADDRESS;
    }
    else{return EE_NO_VALID_PAGE;}

    /* Set the new Page status to RECEIVE_DATA status */
    _flashStatus=EE_FLASH_Write_Half_Word(_newPageAddress,EE_RECEIVE_DATA);
    /* If program operation was failed, a Flash error code is returned */
    if(HAL_OK!=_flashStatus){return _flashStatus;}

    /* Write the variable passed as parameter in the new active page */
    _eepromStatus=EE_Verify_Page_Full_Write_Variable(_virtualAddress,_data);
    /* If program operation was failed, a Flash error code is returned */
    if(HAL_OK!=_eepromStatus){return _eepromStatus;}

    /* Transfer process: transfer variables from old to the new active page */
    for(uint16_t _varIdx=0U;_varIdx<EE_VALUES_NUMBER;_varIdx++)
    {
        if(EE_VIRTUAL_ADDRESS_TABLE[_varIdx]!=_virtualAddress) /* Check each variable except the one passed as parameter */
        {  
            /* Read the other last variable updates */
        	uint16_t _readStatus=EE_Read_Variable(EE_VIRTUAL_ADDRESS_TABLE[_varIdx],&EE_DATA_VARIABLE);
            /* In case variable corresponding to the virtual address was found */
            if(0x01!=_readStatus)
            {
                /* Transfer the variable to the new active page */
                _eepromStatus=EE_Verify_Page_Full_Write_Variable(EE_VIRTUAL_ADDRESS_TABLE[_varIdx],EE_DATA_VARIABLE);
                /* If program operation was failed, a Flash error code is returned */
                if(HAL_OK!=_eepromStatus){return _eepromStatus;}
            }
        }
    }

    /* Erase the old Page: Set old Page status to ERASED status */
    _flashStatus=EE_FLASH_Page_Erase(_oldPageAddress);
    /* If erase operation was failed, a Flash error code is returned */
    if(HAL_OK!=_flashStatus){return _flashStatus;}

    /* Set new Page status to VALID_PAGE status */
    _flashStatus=EE_FLASH_Write_Half_Word(_newPageAddress,EE_VALID_PAGE);
    /* If program operation was failed, a Flash error code is returned */
    if(HAL_OK!=_flashStatus){return _flashStatus;}

    /* Return last operation flash status */
    return _flashStatus;
}

/**
  ***************************************************************************************************************************************
  * @brief  Erase a page in polling mode
  * @param  Page (uint32_t), number of pages (uint16_t)
  * @retval Status (HAL_StatusTypeDef)
  ***************************************************************************************************************************************
  */
static HAL_StatusTypeDef EE_FLASH_Page_Erase(uint32_t _pageAddress)
{
    FLASH_EraseInitTypeDef _eraseInit;
	uint32_t _bank=FLASH_BANK_1, _pageError=0U;

	_eraseInit.TypeErase=FLASH_TYPEERASE_PAGES;
	_eraseInit.NbPages=1U;
	_eraseInit.PageAddress=_pageAddress;
	_eraseInit.Banks=_bank;

    return HAL_FLASHEx_Erase(&_eraseInit,&_pageError);
}

/**
  ***************************************************************************************************************************************
  * @brief  Write a double word at the given address in Flash
  * @param  Address (uint32_t), data (uint64_t)
  * @retval Status (HAL_StatusTypeDef)
  ***************************************************************************************************************************************
  */
static HAL_StatusTypeDef EE_FLASH_Write_Half_Word(uint32_t _address, uint16_t _data)
{
	return HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, _address, _data);
}
