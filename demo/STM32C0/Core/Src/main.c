/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mb.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {

    /* USER CODE BEGIN 1 */
    eMBErrorCode mb_code;
    /* USER CODE END 1 */
    
    /* MCU Configuration--------------------------------------------------------*/
    
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    
    /* USER CODE BEGIN Init */
    
    /* USER CODE END Init */
    
    /* Configure the system clock */
    SystemClock_Config();
    
    /* USER CODE BEGIN SysInit */
    
    /* USER CODE END SysInit */
    
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    
    /* USER CODE BEGIN 2 */
    if (eMBInit(MB_RTU, 10, 1, 19200, MB_PAR_NONE) != MB_ENOERR) {
        Error_Handler();
    }
    
    xMBPortTimersInit(0);
    vMBPortTimersEnable();
    mb_code = eMBEnable();
    (void)xMBPortSerialInit(0, 0, 0, 0); // Always returns TRUE.
    vMBPortSerialEnable(TRUE, TRUE);
    /* USER CODE END 2 */
    
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (mb_code == MB_ENOERR) {
        /* USER CODE END WHILE */
        
        /* USER CODE BEGIN 3 */
        mb_code = eMBPoll();
        HAL_Delay(5000);
    }
    
    for (;;);
    
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
    
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */
    /* USER CODE END MX_GPIO_Init_1 */
    
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_SET);
    
    /*Configure GPIO pin : User_Button_Pin */
    GPIO_InitStruct.Pin = User_Button_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(User_Button_GPIO_Port, &GPIO_InitStruct);
    
    /*Configure GPIO pin : Led_Pin */
    GPIO_InitStruct.Pin = Led_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(Led_GPIO_Port, &GPIO_InitStruct);
    
    /*Configure GPIO pins : PB8 PB9 */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
    
    /* USER CODE BEGIN MX_GPIO_Init_2 */
    /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/*
 * Defined by modbus/mb.c
eMBErrorCode eMBRegisterCB(UCHAR ucFunctionCode,
                           pxMBFunctionHandler pxHandler) {
}
 *
 */

eMBErrorCode eMBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress,
                           USHORT usNRegs) {
    const UCHAR TEST_INPUT = 0xA5;
    eMBErrorCode mb_code = MB_ENOERR;
    
    if (usAddress < 1) { mb_code = MB_ENOREG; }
    
    if ((usAddress + usNRegs - 1) > UINT16_MAX) { mb_code = MB_ENOREG; }
    
    if (MB_ENOERR == mb_code) {
        ///@todo Review https://github.com/VolutionGroup/freemodbus
        /// Commented-out code is from demo/WIN32TCP/demo.cpp#L295
        // iRegIndex = ( int )( usAddress - usRegInputStart );
        while (usNRegs > 0) {
            *pucRegBuffer++ = TEST_INPUT; // (UCHAR)(usRegInputBuf[iRegIndex] >> 8);
            *pucRegBuffer++ = (UCHAR)
                              ~TEST_INPUT; // (UCHAR)(usRegInputBuf[iRegIndex] & 0xFF);
            // iRegIndex++;
            usNRegs--;
        }
    }
    
    ///@todo Add cases for MB_ETIMEDOUT and MB_EIO
    return mb_code;
}

eMBErrorCode eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress,
                             USHORT usNRegs, eMBRegisterMode eMode) {
    eMBErrorCode mb_code = MB_ENOERR;
    
    if (MB_REG_READ == eMode) { mb_code = eMBRegInputCB(pucRegBuffer, usAddress, usNRegs); }
    else { /* Stub of writing without error */ }
    
    return mb_code;
}

eMBErrorCode eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress,
                           USHORT usNCoils, eMBRegisterMode eMode) {
    return eMBRegHoldingCB(pucRegBuffer, usAddress, usNCoils, eMode);
}

eMBErrorCode eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress,
                              USHORT usNDiscrete) {
    const UCHAR TEST_DISCRETE = 0x01;
    eMBErrorCode mb_code = MB_ENOERR;
    
    if (usAddress < 1) { mb_code = MB_ENOREG; }
    
    if ((usAddress + usNDiscrete - 1) > UINT16_MAX) { mb_code = MB_ENOREG; }
    
    if (MB_ENOERR == mb_code) {
        while (usNDiscrete > 0) {
            *pucRegBuffer++ = TEST_DISCRETE; // (UCHAR)(usRegInputBuf[iRegIndex] >> 8);
            *pucRegBuffer++ = ~TEST_DISCRETE; // (UCHAR)(usRegInputBuf[iRegIndex] & 0xFF);
            // iRegIndex++;
            usNDiscrete--;
        }
    }
    
    ///@todo Add cases for MB_ETIMEDOUT and MB_EIO
    return mb_code;
}

eMBErrorCode eMBRegFileCB(UCHAR *pucFileBuffer, USHORT usFileNumber,
                          USHORT usRecordNumber, USHORT usRecordLength,
                          eMBRegisterMode eMode) {
    return MB_ENOREG;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    
    while (1) {
    }
    
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number, */
    /*  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}

#endif /* USE_FULL_ASSERT */
