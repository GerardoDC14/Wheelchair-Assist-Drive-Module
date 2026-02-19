#include <stdint.h>
#include "stm32f051x8.h"
#include "main.h"
#include "mpu6050.h"
#include "fonts.h"
#include "ssd1306.h"
#include "bitmap_tec.h"
#include "adc.h"
#include "display.h"
#include "utilities.h"
#include <math.h>

// Declare volatile variables to handle rotary encoder, pulse counting, and RPM calculations
volatile float rpm = 0.0f;
volatile uint32_t count = 0;

// Declare a global variable to store the start time
volatile uint32_t startTime = 0;
#define RPM_CHANGE_THRESHOLD 1.0f // Adjust this threshold as needed
float lastWheelchairRPM = 0.0f;
int inAutomaticMode = 0; // Flag to indicate if we are in automatic mode

#define AUTO_MODE_TRIGGER_TIME 2000  // Time in milliseconds

uint32_t lastEncoderChangeTime = 0;


// Constants
const uint32_t pulses_per_revolution = 15; // Number of pulses for one complete revolution
const uint32_t PWMAdjustmentStep = 1;
const uint32_t MaxPWMValue = 80;

#define SystemCoreClock 8000000

// Global or static variables for PWM adjustment
static int targetPWM = 25; // Initial PWM value set to 25

#define MOVING_AVERAGE_SIZE 2
volatile float rpmMeasurements[MOVING_AVERAGE_SIZE] = {0.0f};
volatile uint8_t rpmMeasurementIndex = 0;

float CalculateMovingAverage(void);

int CalculatePWMFromRPM(float rpm) {
    float a = -0.04f, b = 19.36f, c = -581.43f - rpm;
    float discriminant = b*b - 4*a*c;
    if (discriminant < 0) return -1; // No real solution

    float pwm1 = (-b + sqrtf(discriminant)) / (2*a);
    float pwm2 = (-b - sqrtf(discriminant)) / (2*a);

    // Select the valid PWM value within the range [25, MaxPWMValue]
    if (pwm1 >= 25 && pwm1 <= MaxPWMValue) {
        return (int)pwm1;
    } else if (pwm2 >= 25 && pwm2 <= MaxPWMValue) {
        return (int)pwm2;
    }
    return -1; // No valid PWM in range
}


// Setup TIM1 for PWM
void setupTimer1PWM(void) {
    RCC->APB2ENR |= (1 << 11); // Enable TIM1 clock
    TIM1->ARR = 79; // Set auto-reload value for 100kHz PWM frequency
    TIM1->CCR2 = 25; // Set duty cycle for channel 2
    TIM1->CCMR1 |= (0b110 << 12); // Set PWM mode 1 for channel 2
    TIM1->CCMR1 |= (1 << 11); // Enable preload for channel 2
    TIM1->CCER |= (1 << 4); // Enable channel 2 output
    TIM1->BDTR |= (1 << 15); // Main output enable
    TIM1->CR1 |= (1 << 7); // Enable auto-reload preload
    TIM1->CR1 &= ~(0x3 << 5); // Set edge-aligned mode
    TIM1->CR1 &= ~(1 << 4); // Set up-counting mode
    TIM1->CR1 |= (1 << 0); // Enable counter
}
// Function to start the RPM measurement
void StartRPMMeasurement(void) {
    count = 0;  // Reset pulse count
    startTime = HAL_GetTick();  // Get the current time in milliseconds
}

// Function to calculate RPM
float CalculateRPM(void) {
    uint32_t currentTime = HAL_GetTick();
    uint32_t elapsedTime = currentTime - startTime;

    if (elapsedTime >= 0.1) { // Example interval
        float calculatedRPM = ((float)count / pulses_per_revolution) * 60.0f * 1000.0f / elapsedTime;
        rpmMeasurements[rpmMeasurementIndex] = calculatedRPM;
        rpmMeasurementIndex = (rpmMeasurementIndex + 1) % MOVING_AVERAGE_SIZE;

        StartRPMMeasurement();
        return CalculateMovingAverage(); // Calculate and return the average RPM
    }
    return rpm; // Return the last calculated RPM
}


float CalculateMovingAverage(void) {
    float sum = 0.0f;
    for (uint8_t i = 0; i < MOVING_AVERAGE_SIZE; i++) {
        sum += rpmMeasurements[i];
    }
    return sum / MOVING_AVERAGE_SIZE;
}


// Initialize GPIOs
void initGPIO(void) {
    RCC->AHBENR |= (1 << 17); // Enable GPIOA clock
    GPIOA->MODER |= (2 << 18); // Set PA9 to alternate function
    GPIOA->AFR[1] |= (2 << 4); // Set AF2 (TIM1_CH2) for PA9
    // Configure PA12 and PA14 as input
    GPIOA->MODER &= ~(3 << (2 * 2)); // Set PA12 to input mode
    GPIOA->MODER &= ~(3 << (14 * 2)); // Set PA14 to input mode

    GPIOA->MODER &= ~(GPIO_MODER_MODER0);  // Set PA0 as alternate function
	GPIOA->MODER |= (1 << 1);
	GPIOA->PUPDR &= ~(1 << 0);
    GPIOA->AFR[0] |= (1 << 1);  // Set alternate function for PA0 to connect with TIM2


    RCC->AHBENR |= (1 << 18); // Enable GPIOB clock
    GPIOB->MODER &= ~(3 << (11 * 2)); // Set PB11 to input mode
    GPIOB->MODER &= ~(3 << (12 * 2)); // Set PB12 to input mode

    // Configure PC0, PC1, and PC2 for ADC operation
    RCC->AHBENR |= (1 << 19); // Enable GPIOC clock
    GPIOC->MODER |= (3 << (0 * 2)); // Set PC0 to analog mode
    GPIOC->MODER |= (3 << (1 * 2)); // Set PC1 to analog mode
    GPIOC->MODER |= (3 << (2 * 2)); // Set PC2 to analog mode

    // Initialize GPIO for Buzzer on PA1
    GPIOA->MODER |= (1 << 2);  // Set PA1 as output
    GPIOA->OTYPER &= ~(1 << 1);  // Set PA1 as push-pull
    GPIOA->OSPEEDR &= ~(3 << 2);  // Set low speed for PA1
    GPIOA->PUPDR &= ~(3 << 2);  // No pull-up or pull-down for PA1
}

// Initialize rotary encoder
void initRotaryEncoder(void) {
    RCC->APB2ENR |= (1 << 0); // Enable SYSCFG clock
    SYSCFG->EXTICR[2] |= (1 << 12); // Connect EXTI11 to PB11
    EXTI->RTSR |= (1 << 11); // Enable rising edge trigger for EXTI11
    EXTI->IMR |= (1 << 11); // Enable EXTI11 interrupt
    NVIC_EnableIRQ(EXTI4_15_IRQn); // Enable EXTI4 to EXTI15 interrupt in NVIC
}

// IRQ handler for EXTI4 to EXTI15
void EXTI4_15_IRQHandler(void) {
    static uint8_t lastState = 0;

    // Read the current state of the encoder pins
    uint8_t currentState = ((GPIOB->IDR & (1 << 11)) >> 11) |
                           ((GPIOB->IDR & (1 << 12)) >> (12 - 1));

    // Determine rotation direction based on state transition
    if ((lastState == 0b01 && currentState == 0b11) ||
        (lastState == 0b11 && currentState == 0b10) ||
        (lastState == 0b10 && currentState == 0b00) ||
        (lastState == 0b00 && currentState == 0b01)) {
        // Clockwise rotation
        targetPWM += PWMAdjustmentStep;
    } else if ((lastState == 0b00 && currentState == 0b10) ||
               (lastState == 0b10 && currentState == 0b11) ||
               (lastState == 0b11 && currentState == 0b01) ||
               (lastState == 0b01 && currentState == 0b00)) {
        // Counter-clockwise rotation
        targetPWM -= PWMAdjustmentStep;
    }

    // Update last state
    lastState = currentState;

    // Clamp the targetPWM within valid range
    if (targetPWM < 25) targetPWM = 25;
    if (targetPWM > MaxPWMValue) targetPWM = MaxPWMValue;

    // Clear interrupt flags
    EXTI->PR |= (1 << 11) | (1 << 12);
}

void TIM2_IRQHandler (void){
	TIM2->SR &= ~(TIM_SR_CC1IF_Msk);

	//Edge Counter
	count++;
}

void EnableClocks(void) {
    RCC->APB1ENR |= (1 << 0); // Enable TIM2 clock
    RCC->AHBENR |= (1 << 17); // Enable GPIOA clock
}

void ConfigureGPIOAForTIM2(void) {
    // Set PA0 as alternate function
    GPIOA->MODER &= ~(GPIO_MODER_MODER0);
    GPIOA->MODER |= (1 << 1);
    GPIOA->PUPDR &= ~(1 << 0);

    // Set alternate function for PA0 to connect with TIM2
    GPIOA->AFR[0] |= (1 << 1);
}

void ConfigureTIM2(void) {
    // Clear CCMR1 register and set CC1 channel as input
    TIM2->CCMR1 = 0;
    TIM2->CCMR1 |= 1 << 0; // CC1S = 0b01 : CC1 channel configured as input, IC1 mapped on TI1
    TIM2->CCMR1 |= (0b0011 << 4); // IC1F = 0b0011 : Filter set to 8 clock cycles

    // Clear CCER register and select rising edge trigger
    TIM2->CCER = 0;
    TIM2->CCER &= ~(TIM_CCER_CC1P_Msk);
    TIM2->CCER |= TIM_CCER_CC1E_Msk; // Enable capture

    TIM2->DIER |= (1 << 1); // Enable Capture/Compare interrupt

    // Enable TIM2 interrupt in NVIC
    NVIC_EnableIRQ(TIM2_IRQn);

    // Enable TIM2 counter
    TIM2->CR1 |= TIM_CR1_CEN_Msk;
}

// Function to calculate speed in km/h from RPM
float CalculateSpeedKmH(float rpm) {
    const float wheelDiameterCm = 16.0f; // Diameter of the wheel in cm
    const float pi = 3.14159265358979323846f;
    float circumferenceKm = (pi * wheelDiameterCm) / 100000.0f; // Circumference in km (1 cm = 0.00001 km)
    return circumferenceKm * rpm * 60.0f; // Speed in km/h
}


I2C_HandleTypeDef hi2c1;


MPU6050_t MPU6050;

static void MX_I2C1_Init(void);

int main(void)
{
   uint16_t adc_value_temp, adc_value_battery, adc_value_current;
   float temperature, battery_voltage, current;

  // Call initialization functions
  initGPIO();
  setupTimer1PWM();
  initRotaryEncoder();
  init_ADC();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  MX_I2C1_Init();
  EnableClocks();
  ConfigureGPIOAForTIM2();
  ConfigureTIM2();
  StartRPMMeasurement();

  while (MPU6050_Init(&hi2c1) == 1);
  SSD1306_Init();
  SSD1306_GotoXY (0,0);
  SSD1306_Puts ("Quinta",&Font_11x18,1);
  SSD1306_GotoXY (10,30);
  SSD1306_Puts ("Rueda",&Font_11x18,1);
  SSD1306_UpdateScreen();

  HAL_Delay (1000);
  SSD1306_Clear();

  SSD1306_DrawBitmap(0,0,logobn,128,64,1);
  SSD1306_UpdateScreen();

  HAL_Delay (2000);
  SSD1306_Clear();

  soundBuzzer(2000);

  while (1) {
	    static uint8_t lastState = 0;

	    // Read the current state of the encoder pins
	    uint8_t currentState = ((GPIOB->IDR & (1 << 11)) >> 11) |
	                           ((GPIOB->IDR & (1 << 12)) >> (12 - 1));

	    // Polling PA2 and PA14
	    int sensorPA2State = GPIOA->IDR & (1 << 2); // Read PA2
	    int sensorPA14State = GPIOA->IDR & (1 << 14); // Read PA14

      int currentPWM = TIM1->CCR2;

      if (lastState != currentState) {
              // Reset the timer if there's a change in the encoder input
              lastEncoderChangeTime = HAL_GetTick();
              inAutomaticMode = 0;  // Switch to manual mode
          } else if (HAL_GetTick() - lastEncoderChangeTime > AUTO_MODE_TRIGGER_TIME) {
              // Switch to automatic mode if there's been no encoder input for a defined duration
              inAutomaticMode = 1;
          }
          lastState = currentState;
      // Calculate current RPM of the fifth wheel
      float currentRPM = CalculateRPM();

      // Manual control logic
      if (!inAutomaticMode) {
          // Increment or decrement PWM based on rotary encoder input
          if (currentPWM < targetPWM) {
              TIM1->CCR2 = targetPWM;
          } else if (currentPWM > targetPWM) {
              TIM1->CCR2 = targetPWM;
          }
      } else {
          // Automatic mode logic
          // Desired RPM is set to be the same as the current RPM
          float desiredRPM = currentRPM+1;
          int requiredPWM = CalculatePWMFromRPM(desiredRPM);
          if (requiredPWM != -1) {
              targetPWM = requiredPWM;
          }
          TIM1->CCR2 = targetPWM;
      }

      // Read ADC values and convert to physical quantities
      adc_value_temp = read_ADC_Channel(10);
      temperature = ADC_to_Temperature(adc_value_temp);
      adc_value_battery = read_ADC_Channel(12);
      battery_voltage = ADC_to_BatteryVoltage(adc_value_battery);
      adc_value_current = read_ADC_Channel(11);
      current = ADC_to_Current(adc_value_current);
      float speedKmH = CalculateSpeedKmH(currentRPM);
      if (currentRPM != rpm) {
          rpm = currentRPM;
      }

      MPU6050_Read_All(&hi2c1, &MPU6050);
      // Call the function to update the display
      UpdateDisplay(currentPWM, MPU6050.KalmanAngleX, MPU6050.KalmanAngleY, temperature, speedKmH,battery_voltage);
  }

}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x0000020B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

}


void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }

}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
