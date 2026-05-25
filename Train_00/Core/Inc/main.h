/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* 升压变换器控制与状态结构体 */
typedef struct {
    /* ==================== 1. 用户配置参数 (可根据需要随时修改) ==================== */
    float input_voltage;      // 输入电压 (V)，例如 6.0f
    float target_voltage;     // 目标输出电压 (V)，例如 12.0f (可由按键步进修改)
    float max_current_limit;  // 最大电流限制 (A)，例如 1.5f (过流保护阈值)
    float kp;                 // PI 控制器比例系数
    float ki;                 // PI 控制器积分系数
    float kd;                 // PI 控制器微分系数
    
    /* ==================== 2. 实时采样与监控数据 (只读/调试监控) ==================== */
    float current_voltage;    // 当前实测输出电压 (V)
    float current_current;    // 当前实测输出电流 (A)
    float current_power;      // 当前实测输出功率 (W) -> P = U * I
    float duty_cycle;         // 当前 PWM 占空比 (0.0% ~ 80.0%)
    uint32_t pwm_ccr;         // 当前 TIM1_CH1 寄存器 CCR 实际写入值 (0 ~ 600)
    
    /* ==================== 3. 系统运行状态 ==================== */
    uint8_t is_fault;         // 故障标志位 (0: 正常, 1: 过流保护触发已关断)
    uint8_t is_soft_starting; // 软启动状态标志位 (0: 正常运行, 1: 正在软启动)
} Boost_Controller_t;

extern Boost_Controller_t g_boost;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED0_Pin GPIO_PIN_13
#define LED0_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
