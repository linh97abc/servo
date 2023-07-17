#ifndef __SERVO_CONTROLLER_REG_H__
#define __SERVO_CONTROLLER_REG_H__

#include <stdint.h>

#define SERVO_CONTROLLER_NUM_SERVO 4

// #define SERVO_CONTROLLER_CR_EN_BIT  (1u << 0)
// #define SERVO_CONTROLLER_CR_FILTER_LEVEL_BIT 1

// #define SERVO_CONTROLLER_CR_PROTECT_EN0_BIT  (1u << 5)
// #define SERVO_CONTROLLER_CR_PROTECT_EN1_BIT  (1u << 6)
// #define SERVO_CONTROLLER_CR_PROTECT_EN2_BIT  (1u << 7)
// #define SERVO_CONTROLLER_CR_PROTECT_EN3_BIT  (1u << 8)

// #define SERVO_CONTROLLER_CR_DRV_EN0_BIT  (1u << 9)
// #define SERVO_CONTROLLER_CR_DRV_EN1_BIT  (1u << 10)
// #define SERVO_CONTROLLER_CR_DRV_EN2_BIT  (1u << 11)
// #define SERVO_CONTROLLER_CR_DRV_EN3_BIT  (1u << 12)

#define SERVO_CONTROLLER_FLAG_DRV8320_FAULT3_BIT  (1u << 10)
#define SERVO_CONTROLLER_FLAG_DRV8320_FAULT2_BIT  (1u << 9)
#define SERVO_CONTROLLER_FLAG_DRV8320_FAULT1_BIT  (1u << 8)
#define SERVO_CONTROLLER_FLAG_DRV8320_FAULT0_BIT  (1u << 7)

#define SERVO_CONTROLLER_FLAG_STOP3_BIT (1u << 6)
#define SERVO_CONTROLLER_FLAG_STOP2_BIT (1u << 5)
#define SERVO_CONTROLLER_FLAG_STOP1_BIT (1u << 4)
#define SERVO_CONTROLLER_FLAG_STOP0_BIT (1u << 3)

#define SERVO_CONTROLLER_FLAG_ADC_VALID_BIT  (1u << 2)
#define SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT  (1u << 1)
#define SERVO_CONTROLLER_FLAG_REALTIME_ERR_BIT  (1u << 0)

#define SERVO_CONTROLLER_TR_RESET_BIT  (1u << 6)
#define SERVO_CONTROLLER_TR_START_SERVO3_BIT  (1u << 5)
#define SERVO_CONTROLLER_TR_START_SERVO2_BIT  (1u << 4)
#define SERVO_CONTROLLER_TR_START_SERVO1_BIT  (1u << 3)
#define SERVO_CONTROLLER_TR_START_SERVO0_BIT  (1u << 2)
#define SERVO_CONTROLLER_TR_ADC_INIT_BIT  (1u << 1)
#define SERVO_CONTROLLER_TR_U_VALID_BIT  (1u << 0)

#ifdef __cplusplus
extern "C" {
#endif

typedef union 
{
    struct
    {
        uint32_t en:1;
        uint32_t filter_level:4;
        uint32_t protected_en:4;
        uint32_t drv_en:4;
    } field;
    uint32_t val;
} servo_controller_reg_CR;

typedef union 
{
    struct
    {
        uint32_t realtime_err:1;
        uint32_t mea_trig:1;
        uint32_t adc_valid:1;
        uint32_t drv_stop:4;
        uint32_t drv_fault:4;
    } field;
    uint32_t val;
} servo_controller_reg_FLAG;

typedef union 
{
    struct
    {
        uint32_t realtime_err:1;
        uint32_t mea_trig:1;
        uint32_t adc_valid:1;
        uint32_t drv_stop:4;
        uint32_t drv_fault:4;
    } field;
    uint32_t val;
} servo_controller_reg_IE;

enum Servo_controller_drv_mode_t
{
    SERVO_CONTROLLER_DRV_MODE_6x = 0,
    SERVO_CONTROLLER_DRV_MODE_3x = 1,
    SERVO_CONTROLLER_DRV_MODE_1x = 2,
};

typedef union 
{
    enum Servo_controller_drv_mode_t enum_val;
    uint32_t u32_val;
} servo_controller_reg_DRV_MODE;

typedef union 
{
    int16_t i16_val;
    uint32_t u32_val;
} servo_controller_reg_DUTY;

struct servo_controller_reg_t
{
    volatile servo_controller_reg_CR cr;
    volatile uint32_t tr;
    volatile servo_controller_reg_IE ie;
    volatile servo_controller_reg_FLAG flag;

    volatile uint32_t spi_res;
    volatile uint32_t pwm_pres;
    volatile uint32_t pwm_hperiod;

    volatile servo_controller_reg_DRV_MODE drv_mode[SERVO_CONTROLLER_NUM_SERVO];
    volatile servo_controller_reg_DUTY duty[SERVO_CONTROLLER_NUM_SERVO];
    volatile uint32_t i_max[SERVO_CONTROLLER_NUM_SERVO];
    const volatile uint32_t i[SERVO_CONTROLLER_NUM_SERVO];
    const volatile uint32_t pos[SERVO_CONTROLLER_NUM_SERVO];
};

#ifdef __cplusplus
}
#endif


#endif // __SERVO_CONTROLLER_REG_H__