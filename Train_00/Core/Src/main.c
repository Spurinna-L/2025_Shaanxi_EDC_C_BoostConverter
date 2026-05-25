/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : ????????? ???
  *   - ADC????????(CH0??,CH1??)
  *   - PID????PWM???
  *   - ???
  *   - ??/???? + 3????????
  *   - ????/??????(??while??,???)
  *   - SSD1306 OLED????/??/??/??
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

/* =========================================================
 * SSD1306 OLED ??(I2C,128x64,??????)
 * ========================================================= */
#define OLED_ADDR  0x78
#define OLED_CMD   0x00
#define OLED_DATA  0x40
 float err ;

static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // 0x20 space
    {0x00,0x00,0x5F,0x00,0x00}, // !
    {0x00,0x07,0x00,0x07,0x00}, // "
    {0x14,0x7F,0x14,0x7F,0x14}, // #
    {0x24,0x2A,0x7F,0x2A,0x12}, // $
    {0x23,0x13,0x08,0x64,0x62}, // %
    {0x36,0x49,0x55,0x22,0x50}, // &
    {0x00,0x05,0x03,0x00,0x00}, // '
    {0x00,0x1C,0x22,0x41,0x00}, // (
    {0x00,0x41,0x22,0x1C,0x00}, // )
    {0x14,0x08,0x3E,0x08,0x14}, // *
    {0x08,0x08,0x3E,0x08,0x08}, // +
    {0x00,0x50,0x30,0x00,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x60,0x60,0x00,0x00}, // .
    {0x20,0x10,0x08,0x04,0x02}, // /
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x00,0x36,0x36,0x00,0x00}, // :
    {0x00,0x56,0x36,0x00,0x00}, // ;
    {0x08,0x14,0x22,0x41,0x00}, // <
    {0x14,0x14,0x14,0x14,0x14}, // =
    {0x00,0x41,0x22,0x14,0x08}, // >
    {0x02,0x01,0x51,0x09,0x06}, // ?
    {0x32,0x49,0x79,0x41,0x3E}, // @
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x3F,0x40,0x38,0x40,0x3F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43}, // Z
    {0x00,0x7F,0x41,0x41,0x00}, // [
    {0x02,0x04,0x08,0x10,0x20}, // backslash
    {0x00,0x41,0x41,0x7F,0x00}, // ]
    {0x04,0x02,0x01,0x02,0x04}, // ^
    {0x40,0x40,0x40,0x40,0x40}, // _
    {0x00,0x01,0x02,0x04,0x00}, // `
    {0x20,0x54,0x54,0x54,0x78}, // a
    {0x7F,0x48,0x44,0x44,0x38}, // b
    {0x38,0x44,0x44,0x44,0x20}, // c
    {0x38,0x44,0x44,0x48,0x7F}, // d
    {0x38,0x54,0x54,0x54,0x18}, // e
    {0x08,0x7E,0x09,0x01,0x02}, // f
    {0x0C,0x52,0x52,0x52,0x3E}, // g
    {0x7F,0x08,0x04,0x04,0x78}, // h
    {0x00,0x44,0x7D,0x40,0x00}, // i
    {0x20,0x40,0x44,0x3D,0x00}, // j
    {0x7F,0x10,0x28,0x44,0x00}, // k
    {0x00,0x41,0x7F,0x40,0x00}, // l
    {0x7C,0x04,0x18,0x04,0x78}, // m
    {0x7C,0x08,0x04,0x04,0x78}, // n
    {0x38,0x44,0x44,0x44,0x38}, // o
    {0x7C,0x14,0x14,0x14,0x08}, // p
    {0x08,0x14,0x14,0x18,0x7C}, // q
    {0x7C,0x08,0x04,0x04,0x08}, // r
    {0x48,0x54,0x54,0x54,0x20}, // s
    {0x04,0x3F,0x44,0x40,0x20}, // t
    {0x3C,0x40,0x40,0x20,0x7C}, // u
    {0x1C,0x20,0x40,0x20,0x1C}, // v
    {0x3C,0x40,0x30,0x40,0x3C}, // w
    {0x44,0x28,0x10,0x28,0x44}, // x
    {0x0C,0x50,0x50,0x50,0x3C}, // y
    {0x44,0x64,0x54,0x4C,0x44}, // z
    {0x00,0x08,0x36,0x41,0x00}, // {
    {0x00,0x00,0x7F,0x00,0x00}, // |
    {0x00,0x41,0x36,0x08,0x00}, // }
    {0x10,0x08,0x08,0x10,0x08}, // ~
};

static uint8_t oled_buf[1024];

static void OLED_WriteCmd(uint8_t cmd)
{
    uint8_t buf[2] = {OLED_CMD, cmd};
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, buf, 2, 10);
}

static void OLED_Init(void)
{
    HAL_Delay(100);
    OLED_WriteCmd(0xAE);
    OLED_WriteCmd(0x20); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xB0);
    OLED_WriteCmd(0xC8);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0x10);
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x81); OLED_WriteCmd(0xFF);
    OLED_WriteCmd(0xA1);
    OLED_WriteCmd(0xA6);
    OLED_WriteCmd(0xA8); OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xA4);
    OLED_WriteCmd(0xD3); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xD5); OLED_WriteCmd(0xF0);
    OLED_WriteCmd(0xD9); OLED_WriteCmd(0x22);
    OLED_WriteCmd(0xDA); OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xDB); OLED_WriteCmd(0x20);
    OLED_WriteCmd(0x8D); OLED_WriteCmd(0x14);
    OLED_WriteCmd(0xAF);
}

static void OLED_Clear(void)
{
    memset(oled_buf, 0, sizeof(oled_buf));
}

static void OLED_Flush(void)
{
    OLED_WriteCmd(0x21); OLED_WriteCmd(0); OLED_WriteCmd(127);
    OLED_WriteCmd(0x22); OLED_WriteCmd(0); OLED_WriteCmd(7);
    uint8_t tmp[17];
    tmp[0] = OLED_DATA;
    for (int i = 0; i < 1024; i += 16)
    {
        memcpy(&tmp[1], &oled_buf[i], 16);
        HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, tmp, 17, 10);
    }
}

static void OLED_DrawChar(uint8_t page, uint8_t col, char ch, uint8_t scale)
{
    if (ch < 0x20 || ch > 0x7E) ch = ' ';
    const uint8_t *glyph = font5x7[ch - 0x20];
    for (int cx = 0; cx < 5; cx++)
    {
        uint8_t col_data = glyph[cx];
        for (int sx = 0; sx < scale; sx++)
        {
            uint16_t x = col + cx * scale + sx;
            if (x >= 128) break;
            for (int bit = 0; bit < 7; bit++)
            {
                if (col_data & (1 << bit))
                {
                    for (int sy = 0; sy < scale; sy++)
                    {
                        uint16_t y  = page * 8 + bit * scale + sy;
                        uint16_t pg = y / 8;
                        if (pg < 8)
                            oled_buf[pg * 128 + x] |= (1 << (y % 8));
                    }
                }
            }
        }
    }
}

static void OLED_DrawStr(uint8_t page, uint8_t col,
                         const char *str, uint8_t scale)
{
    while (*str)
    {
        OLED_DrawChar(page, col, *str, scale);
        col += (5 + 1) * scale;
        str++;
        if (col >= 128) break;
    }
}

/* =========================================================
 * 系统全局变量与缓冲区
 * ========================================================= */
volatile uint16_t adc_buffer[2] = {0}; // ADC DMA 缓冲区: [0]为电压, [1]为电流

/* --- 结构体 & 控制器 --- */
Boost_Controller_t g_boost = {
    .input_voltage = 6.0f,       // 输入电压 6V
    .target_voltage = 15.0f,     // 默认目标输出电压 15V (10~15V, 步进0.5V)
    .max_current_limit = 1.5f,   // 限制最大电流 1.5A
    .kp = 2.0f,                  // 比例系数
    .ki = 0.5f,                  // 积分系数
    .kd = 0.0f,                  // 微分系数
    
    .current_voltage = 0.0f,
    .current_current = 0.0f,
    .current_power = 0.0f,
    .duty_cycle = 0.0f,
    .pwm_ccr = 10,
    
    .is_fault = 0,
    .is_soft_starting = 1
};

float target_voltage      = 15.0f;   // ??????(10~15V,??0.5V)
float soft_target_voltage = 0.0f;    // ????????

/* --- ??? --- */
float actual_voltage = 0.0f;
float actual_current = 0.0f;
float actual_power   = 0.0f;

/* --- ADC??????? --- */
float voltage_filtered = 0.0f;
float current_filtered = 0.0f;

/* --- PID参数 (从结构体同步) --- */
float Kp = 10.0f;
float Ki = 0.0001f; // 引入积分系数，消除静差
float Kd = 0.0f;

float integral_error = 0.0f;
float last_error     = 0.0f;

/* --- PWM (TIM1 ARR = 719, 100kHz) --- */
#define MAX_DUTY 576  // 80% 占空比限制 (720 * 0.8)
#define MIN_DUTY  36  // 5% 占空比限制 (720 * 0.05)
int32_t pwm_duty = 36;

/* --- 保护与计数器 --- */
uint8_t  ocp_triggered    = 0;
uint32_t ocp_trigger_time = 0;
#define  OCP_LOCKOUT_MS   3000   // 保护锁定3秒

uint32_t ocp_count = 0;
uint32_t ovp_count = 0;
uint32_t stuck_count = 0;
int32_t last_pwm_duty = 36;

/* --- 刷新计数 --- */
uint32_t display_tick = 0;

/* --- 按键状态 (1表示未按下，0表示按下) --- */
uint8_t key0_last = 1;  // PB0
uint8_t key1_last = 1;  // PB1
uint8_t key10_last = 1; // PB10

/* =========================================================
 * 函数声明
 * ========================================================= */
void SystemClock_Config(void);
float LowPassFilter(float old_val, float new_val);
void Read_System_Status(void);
void Soft_Start(void);
void Current_Limit(void);
void PID_Controller(void);
void Check_Protection(void);
void Key_Scan(void);
void Update_Display(void);

/* =========================================================
 * 核心控制函数
 * ========================================================= */
float LowPassFilter(float old_val, float new_val)
{
    return new_val ;
}

/* =========================================================
 * ADC 轮询采样换算与滤波 (放弃不稳定的 DMA)
 * ========================================================= */
void Read_System_Status(void)
{
    // 1. 启动 ADC 转换
    HAL_ADC_Start(&hadc1);

    // 2. 等待转换完成 (超时时间 10ms，实际只需不到 2us)
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
    {
        // 3. 读取 ADC 原始值
        uint32_t adc_val = HAL_ADC_GetValue(&hadc1);

        // 4. 标定与修正系数
        // 之前由于 ADC 采样时间过短导致读数偏小，现在已在 adc.c 中延长了采样时间。
        // 暂时将修正系数恢复为 1.0f，观察真实的硬件读数。
        float cal_factor = 1.0f; 

        // 5. 电压换算 (10k/2k 分压，分压比为 6.0)
        float raw_v = ((float)adc_val / 4095.0f) * 3.3f * cal_factor * 6.0f;
        
        // 6. 滤波
        voltage_filtered = LowPassFilter(voltage_filtered, raw_v);
        actual_voltage   = voltage_filtered;
        g_boost.current_voltage = actual_voltage *8.08f/4.19f;
    }

    // 7. 停止 ADC
    HAL_ADC_Stop(&hadc1);

    // 电流通道暂未连接，强制设为 0.0A
    float raw_i = 0.0f;
    current_filtered = LowPassFilter(current_filtered, raw_i);
    actual_current   = current_filtered;
    g_boost.current_current = actual_current;

    // 计算功率
    actual_power = actual_voltage * actual_current;
    g_boost.current_power = actual_power;
}
void Soft_Start(void)
{
    // 同步用户可能在结构体中修改的目标电压
    target_voltage = g_boost.target_voltage;
    Kp = g_boost.kp;
    Ki = g_boost.ki;
    Kd = g_boost.kd;

    if (soft_target_voltage < target_voltage)
    {
        soft_target_voltage += 0.002f; // 放缓软启动步长，让实际电压能跟上
        if (soft_target_voltage > target_voltage)
            soft_target_voltage = target_voltage;
        g_boost.is_soft_starting = 1;
    }
    else if (soft_target_voltage > target_voltage)
    {
        soft_target_voltage = target_voltage;
        g_boost.is_soft_starting = 0;
    }
    else
    {
        g_boost.is_soft_starting = 0;
    }
}

/* =========================================================
 * 限流保护：当电流超过限制时，强制减小占空比
 * ========================================================= */
// void Current_Limit(void)
// {
//     if (actual_current > g_boost.max_current_limit)
//     {
//         pwm_duty -= 5;
//         if (pwm_duty < MIN_DUTY) pwm_duty = MIN_DUTY;
//         __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_duty);
//         g_boost.pwm_ccr = pwm_duty;
//         g_boost.duty_cycle = (float)pwm_duty / 720.0f; // TIM1 Period = 719
//     }
// }

/* =========================================================
 * PID 控制器
 * ========================================================= */
#define  STUCK_THRESHOLD 500  // 500ms

void PID_Controller(void)
{
    if (ocp_triggered)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
        g_boost.pwm_ccr = 0;
        g_boost.duty_cycle = 0.0f;
        g_boost.is_fault = 1;
        return;
    }
    else
    {
        g_boost.is_fault = 0;
    }

    float error      = soft_target_voltage - actual_voltage;
    float derivative = error - last_error;
    last_error       = error;

    // 允许在全电压范围内进行积分累加，取消“误差大于3V清零”的限制
    integral_error += error;
    
    // 积分限幅 (Anti-Windup)，放宽积分上限，允许占空比推得更高

    // 以最小占空比 MIN_DUTY 为基准进行 PI 调节
    float output = MIN_DUTY 
                 + Kp * error
                 + Ki * integral_error
                 + Kd * derivative;

    if (output > MAX_DUTY) output = MAX_DUTY;
    if (output < MIN_DUTY) output = MIN_DUTY;

    int32_t new_duty = (int32_t)output;

    // 限制占空比变化率，防止MOS管过载
    if (new_duty > pwm_duty + 2)
        new_duty = pwm_duty + 2;

    if (new_duty > MAX_DUTY) new_duty = MAX_DUTY;
    if (new_duty < MIN_DUTY) new_duty = MIN_DUTY;

    // stuck检测
    if ((int32_t)output >= MAX_DUTY && error > 2.0f)
    {
        stuck_count++;
        if (stuck_count >= STUCK_THRESHOLD)
        {
            stuck_count    = 0;
            integral_error = 0.0f;
            last_error     = 0.0f;
        }
    }
    else
    {
        stuck_count = 0;
    }

    last_pwm_duty = new_duty;
    pwm_duty      = new_duty;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_duty);
    g_boost.pwm_ccr = pwm_duty;
    g_boost.duty_cycle = (float)pwm_duty / 720.0f; // TIM1 Period = 719
    err = integral_error;
}

void Check_Protection(void)
{
    if (ocp_triggered) return;

    // 暂不连接电流检测模块时，不进行过流保护判定，防止悬空引脚噪声触发保护
    /*
    if (actual_current >= g_boost.max_current_limit)
    {
        ocp_count++;
        if (ocp_count >= 5)
        {
            ocp_count        = 0;
            ocp_triggered    = 1;
            g_boost.is_fault = 1;
            ocp_trigger_time = HAL_GetTick();
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            g_boost.pwm_ccr = 0;
            g_boost.duty_cycle = 0.0f;
        }
    }
    else
    {
        ocp_count = 0;
    }
    */

    if (actual_voltage >= 16.5f)
    {
        ovp_count++;
        if (ovp_count >= 5)
        {
            ovp_count        = 0;
            ocp_triggered    = 1;
            g_boost.is_fault = 1;
            ocp_trigger_time = HAL_GetTick();
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            g_boost.pwm_ccr = 0;
            g_boost.duty_cycle = 0.0f;
        }
    }
    else
    {
        ovp_count = 0;
    }
}

/* =========================================================
 * ????(????,??while??,????)
 *   PB0  ? ???? +0.5V
 *   PB1  ? ???? -0.5V
 *   PB10 ? ????(OCP???3????)
 * ========================================================= */
void Key_Scan(void)
{
    // 暂未连接按键时，直接返回，防止悬空引脚抖动误触发按键逻辑
    return;

    static uint32_t key0_press_time = 0;
    static uint32_t key1_press_time = 0;
    static uint32_t key10_press_time = 0;

    uint8_t key0  = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
    uint8_t key1  = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
    uint8_t key10 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);

    // PB0: 设定电压增加 (带软件消抖)
    if (key0 == GPIO_PIN_RESET)
    {
        if (key0_last == GPIO_PIN_SET)
        {
            key0_press_time = HAL_GetTick();
        }
        else if (HAL_GetTick() - key0_press_time >= 20) // 20ms 消抖
        {
            g_boost.target_voltage += 0.5f;
            if (g_boost.target_voltage > 15.0f) g_boost.target_voltage = 15.0f;
            target_voltage = g_boost.target_voltage;
            if (soft_target_voltage > actual_voltage)
                soft_target_voltage = actual_voltage;
            key0_press_time = HAL_GetTick() + 200; // 连按间隔 200ms
        }
    }

    // PB1: 设定电压减小 (带软件消抖)
    if (key1 == GPIO_PIN_RESET)
    {
        if (key1_last == GPIO_PIN_SET)
        {
            key1_press_time = HAL_GetTick();
        }
        else if (HAL_GetTick() - key1_press_time >= 20) // 20ms 消抖
        {
            g_boost.target_voltage -= 0.5f;
            if (g_boost.target_voltage < 10.0f) g_boost.target_voltage = 10.0f;
            target_voltage = g_boost.target_voltage;
            soft_target_voltage = target_voltage;
            key1_press_time = HAL_GetTick() + 200; // 连按间隔 200ms
        }
    }

    // PB10: 故障恢复按键 (带软件消抖)
    if (key10 == GPIO_PIN_RESET)
    {
        if (key10_last == GPIO_PIN_SET)
        {
            key10_press_time = HAL_GetTick();
        }
        else if (HAL_GetTick() - key10_press_time >= 20) // 20ms 消抖
        {
            if (ocp_triggered && (HAL_GetTick() - ocp_trigger_time >= OCP_LOCKOUT_MS))
            {
                ocp_triggered       = 0;
                g_boost.is_fault    = 0;
                integral_error      = 0.0f;
                last_error          = 0.0f;
                pwm_duty            = MIN_DUTY;
                soft_target_voltage = 0.0f;
                ocp_count           = 0;
                ovp_count           = 0;
            }
            key10_press_time = HAL_GetTick() + 500; // 连按间隔 500ms
        }
    }

    // 更新按键历史状态
    key0_last  = key0;
    key1_last  = key1;
    key10_last = key10;
}

/* =========================================================
 * OLED??(?100ms????)
 * ??:???? / ???? / ???? / ?? / ??
 * ========================================================= */
void Update_Display(void)
{
    char line[24];
    OLED_Clear();

    // 1. 设定电压
    snprintf(line, sizeof(line), "Set: %4.1f V", g_boost.target_voltage);
    OLED_DrawStr(0, 0, line, 1);

    // 2. 当前输出电压
    snprintf(line, sizeof(line), "Uout:%5.2f V", g_boost.current_voltage);
    OLED_DrawStr(1, 0, line, 1);

    // 3. 采样通道电压 (即单片机引脚实际输入的电压，为输出电压的 1/6)
    float adc_pin_voltage = g_boost.current_voltage / 6.0f;
    snprintf(line, sizeof(line), "Uadc:%5.3f V", adc_pin_voltage);
    OLED_DrawStr(2, 0, line, 1);

    // 4. 采样电流 (实测输出电流)
    snprintf(line, sizeof(line), "Iout:%5.3f A", g_boost.current_current);
    OLED_DrawStr(4, 0, line, 1);

    // 5. 功率
    snprintf(line, sizeof(line), "err:   %5.2f W", err);
    OLED_DrawStr(5, 0, line, 1);

    // 6. 状态与占空比
    if (g_boost.is_fault)
    {
        uint32_t elapsed = HAL_GetTick() - ocp_trigger_time;
        if (elapsed < OCP_LOCKOUT_MS)
        {
            uint32_t remain = (OCP_LOCKOUT_MS - elapsed) / 1000 + 1;
            snprintf(line, sizeof(line), "!OCP %lus->RST", remain);
        }
        else
        {
            snprintf(line, sizeof(line), "!OCP PressRST");
        }
    }
    else
    {
        snprintf(line, sizeof(line), "OK  D:%5.1f%%", g_boost.duty_cycle * 100.0f);
    }
    OLED_DrawStr(7, 0, line, 1);

    OLED_Flush();
}

/* =========================================================
 * 1ms 定时器中断服务回调函数 (前台控制环路)
 * ========================================================= */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        // 1. 读取并更新系统状态 (ADC DMA 换算与滤波)
        Read_System_Status();
        
        // 2. 软启动计算
        Soft_Start();
        
        // 3. 保护判定（过流/过压）
        Check_Protection();
        
        // 4. 闭环 PI 控制
        PID_Controller();
    }
}

/* =========================================================
 * main
 * ========================================================= */
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_ADC1_Init();
    HAL_ADCEx_Calibration_Start(&hadc1);  // ADC自校准
    MX_I2C1_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();                       // 初始化 TIM2 (1ms 定时器)
    MX_USART1_UART_Init();

    // 放弃 DMA，改为在 1ms 中断中轮询采样
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 1);

    // OLED初始化
    OLED_Init();
    OLED_Clear();
    OLED_DrawStr(0, 0, "Boost DC-DC", 1);
    OLED_DrawStr(2, 0, "Initializing..", 1);
    OLED_Flush();
    HAL_Delay(500);

    // 启动 PWM 输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    __HAL_TIM_MOE_ENABLE(&htim1);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_duty);

    // 启动 1ms 定时器中断
    HAL_TIM_Base_Start_IT(&htim2);

    uint32_t led_tick = 0;

    while (1)
    {
        // 1. 按键扫描 (后台低优先级)
        Key_Scan();

        // 2. 每 100ms 刷新一次 OLED 屏幕
        display_tick++;
        if (display_tick >= 100)
        {
            display_tick = 0;
            Update_Display();
        }

        // 3. PC13 LED 状态指示
        led_tick++;
        if (g_boost.is_fault)
        {
            // 故障状态：LED0 常亮 (PC13 输出低电平点亮)
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        }
        else
        {
            // 正常运行状态：500ms 闪烁一次
            if (led_tick >= 500)
            {
                led_tick = 0;
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            }
        }

        HAL_Delay(1);  // 延时 1ms
    }
}

/* =========================================================
 * ????(HSE 8MHz � PLL9 = 72MHz)
 * ========================================================= */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct   = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct   = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
        Error_Handler();

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection    = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) Error_Handler();
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) {}
}
