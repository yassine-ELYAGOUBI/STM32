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


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx.h"
#include "stdio.h"
#include "string.h"
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

//static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void initTim2(void)
{
    //ENABLE TIM2 CLOCK
    RCC->APB1ENR |= (1u << 0);

    //LEAVE THE COUNTER FREQUENCY UNCHANGED
    TIM2->PSC = 0;

    //SET TIMER RELOAD VALUE
    TIM2->ARR = (uint32_t)4000000;

    //SET INITIAL COUNTER VALUE
    TIM2->CNT = 0;

    //ENABLE TIM2 COUNTER
    TIM2->CR1 |= (1u << 0);
}

void InitClocks(void)
{
  RCC->APB2ENR |= (1u << 2);     //ENABLE GPIOA CLOCK

  RCC->APB2ENR |= (1u << 12);    //ENABLE SPI1 CLOCK
}
void SetPinMode(void)
{
    //RESET PIN MODES
    GPIOA->CRL &= ~((16u << (4 * 4))       //CLEAR PA4
                     |(16u << (4 * 5))      //CLEAR PA5
                     |(16u << (4 * 6))      //CLEAR PA6
					 |(16u << (4 * 7))      //CLEAR PA7
                     );



    //CONFIGURE PIN MODES
    GPIOA->CRL |= ((1u << (4 * 4))       //CLEAR PA4
                         |(10u << (4 * 5))      //CLEAR PA5
                         |(4u << (4 * 6))      //CLEAR PA6
    					 |(10u << (4 * 7))      //CLEAR PA7
                         );
}
void ConfigSpi(void)
{
    //CONFIGURE SPI1_CR1 REGISTER
    //CLEAR BITS
    SPI1->CR1 &= ~((1u << 15)           //FULL DUPLEX MODE
                  |(1u << 13)           //NOT INTERESTED IN CRC CLACULATIONS, DISABLE CRC
                  |(1u << 10)           //NOT INTERESTED IN SIMPLEX MODE
                  |(1u << 9)            //HARDWARE SLAVE MANAGEMENT
                  |(1u << 7)            //MSB FIRST
                  |(7u << 3)            //RESET BITS THAT DIVIDE THE FREQUENCY BEFORE SETTING THEM AGAIN
                  );

    //SET BITS
    SPI1->CR1 |= ((5u << 3)             //DIVIDE SPI FREQUENCY BY 64
                 |(1u << 2)             //MASTER MODE
                 |(1u << 1)             //CLOCK POLARITY OF 1
                 |(1u << 0)             //CLOCK PHASE OF 1
                 );


    //CONFIGURE SPI1_CR2 REGISTER
    //CLEAR BITS
    SPI1->CR2 &= ~((7u << 5)            //CLEAR INTERRUPT RELATED BITS AS WON'T BE USING INTERRUPTS
                  |(3u << 0)            //WON'T BE USING DMA, DISABLE TX AND RX DMA REQUEST GENERATION
                  );

    //SET BITS
    //SPI1->CR2 |= (
    //             (1u << 2)             //SLAVE SELECT OUTPUT ENABLED
    //             );
    //ENABLE SPI1
    SPI1->CR1 |= (1u << 6);
}

uint8_t SPI_READ(uint8_t address)    //HOLDS THE MPU9250 REGISTER ADDRESS TO REQUEST DATA FROM
{
    uint8_t rx_data = 0;

    //INSTRUCTION OF READ FROM 25LC040
    uint8_t instruction = 3;
    //SET SLAVE SELECT LOW
    GPIOA->ODR &= ~(1u << 4);

    //WRITE INSTRUCTION, ADDRESS AND DUMMY BYTE TO DATA REGISTER

    SPI1->DR = (SPI1->DR & 0xFF00) | address;
    SPI1->DR = (SPI1->DR & 0x00FF) | (instruction << 8);

    //WAIT UNTIL SPI IS NOT BUSY AND RX BUFFER IS NOT EMPTY
    while( ((SPI1->SR)&(1u << 7)) || (!((SPI1->SR)&(1u << 0))) );

    //READ A BYTE FROM THE RX BUFFER
    rx_data = (uint8_t)SPI1->DR;

    //SET SLAVE SELECT HIGH
    GPIOB->ODR |= (1u << 1);

    return rx_data;
}

void InitSPI(void)
{
   //INIT ALL REQUIRED CLOCKS FOR SPI1
	InitClocks();

   //CONFIGURE PINS FOR SPI1
	SetPinMode();

   //INITIALISE SLAVE SELECT HIGH
   GPIOB->ODR |= (1u << 0);

   //CONFIGURE SPI1
   	ConfigSpi();
}































/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/



  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */


  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */





  uint8_t address =187;
  uint8_t DataStored = 0;
  uint8_t MSG[30];

  //SET UP THE TIMER
  initTim2();

  InitSPI();



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  DataStored= SPI_READ(address);

	  sprintf((char *)MSG, "\n DATA= %d Hex\r\n", DataStored);
	  //HAL_UART_Transmit(&huart1,(uint8_t *) MSG, 30, 200);

	  HAL_Delay(100);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
*/
