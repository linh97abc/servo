#ifndef __SERVO_CONTROLLER_REG_H__
#define __SERVO_CONTROLLER_REG_H__

#include <stdint.h>

#define SERVO_CONTROLLER_NUM_SERVO 4

#define SERVO_CONTROLLER_CR_OFFSET 0
#define SERVO_CONTROLLER_TR_OFFSET 1
#define SERVO_CONTROLLER_IE_OFFSET 2
#define SERVO_CONTROLLER_FLAG_OFFSET 3

#define SERVO_CONTROLLER_SPI_PRES_OFFSET 4
#define SERVO_CONTROLLER_PWM_PRES_OFFSET 5
#define SERVO_CONTROLLER_PWM_HPERIOD_OFFSET 6

#define SERVO_CONTROLLER_PULSE_MODE0_OFFSET 7
#define SERVO_CONTROLLER_PULSE_MODE1_OFFSET (SERVO_CONTROLLER_PULSE_MODE0_OFFSET + 1)
#define SERVO_CONTROLLER_PULSE_MODE2_OFFSET (SERVO_CONTROLLER_PULSE_MODE0_OFFSET + 2)
#define SERVO_CONTROLLER_PULSE_MODE3_OFFSET (SERVO_CONTROLLER_PULSE_MODE0_OFFSET + 3)

#define SERVO_CONTROLLER_U0_OFFSET 11
#define SERVO_CONTROLLER_U1_OFFSET (SERVO_CONTROLLER_U0_OFFSET + 1)
#define SERVO_CONTROLLER_U2_OFFSET (SERVO_CONTROLLER_U0_OFFSET + 2)
#define SERVO_CONTROLLER_U3_OFFSET (SERVO_CONTROLLER_U0_OFFSET + 3)

#define SERVO_CONTROLLER_I0_MAX_OFFSET 15
#define SERVO_CONTROLLER_I1_MAX_OFFSET (SERVO_CONTROLLER_I0_MAX_OFFSET + 1)
#define SERVO_CONTROLLER_I2_MAX_OFFSET (SERVO_CONTROLLER_I0_MAX_OFFSET + 2)
#define SERVO_CONTROLLER_I3_MAX_OFFSET (SERVO_CONTROLLER_I0_MAX_OFFSET + 3)

#define SERVO_CONTROLLER_I0_OFFSET 19
#define SERVO_CONTROLLER_I1_OFFSET (SERVO_CONTROLLER_I0_OFFSET + 1)
#define SERVO_CONTROLLER_I2_OFFSET (SERVO_CONTROLLER_I0_OFFSET + 2)
#define SERVO_CONTROLLER_I3_OFFSET (SERVO_CONTROLLER_I0_OFFSET + 3)

#define SERVO_CONTROLLER_POS0_OFFSET 23
#define SERVO_CONTROLLER_POS1_OFFSET (SERVO_CONTROLLER_POS0_OFFSET + 1)
#define SERVO_CONTROLLER_POS2_OFFSET (SERVO_CONTROLLER_POS0_OFFSET + 2)
#define SERVO_CONTROLLER_POS3_OFFSET (SERVO_CONTROLLER_POS0_OFFSET + 3)

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

#define SERVO_CONTROLLER_FLAG_DRV8320_FAULT3_BIT (1u << 10)
#define SERVO_CONTROLLER_FLAG_DRV8320_FAULT2_BIT (1u << 9)
#define SERVO_CONTROLLER_FLAG_DRV8320_FAULT1_BIT (1u << 8)
#define SERVO_CONTROLLER_FLAG_DRV8320_FAULT0_BIT (1u << 7)

#define SERVO_CONTROLLER_FLAG_STOP3_BIT (1u << 6)
#define SERVO_CONTROLLER_FLAG_STOP2_BIT (1u << 5)
#define SERVO_CONTROLLER_FLAG_STOP1_BIT (1u << 4)
#define SERVO_CONTROLLER_FLAG_STOP0_BIT (1u << 3)

#define SERVO_CONTROLLER_FLAG_ADC_VALID_BIT (1u << 2)
#define SERVO_CONTROLLER_FLAG_MEA_TRIG_BIT (1u << 1)
#define SERVO_CONTROLLER_FLAG_REALTIME_ERR_BIT (1u << 0)

#define SERVO_CONTROLLER_TR_RESET_BIT (1u << 6)
#define SERVO_CONTROLLER_TR_START_SERVO3_BIT (1u << 5)
#define SERVO_CONTROLLER_TR_START_SERVO2_BIT (1u << 4)
#define SERVO_CONTROLLER_TR_START_SERVO1_BIT (1u << 3)
#define SERVO_CONTROLLER_TR_START_SERVO0_BIT (1u << 2)
#define SERVO_CONTROLLER_TR_ADC_INIT_BIT (1u << 1)
#define SERVO_CONTROLLER_TR_U_VALID_BIT (1u << 0)

#ifdef __cplusplus
extern "C"
{
#endif

    typedef union
    {
        struct
        {
            uint32_t en : 1;
            uint32_t filter_level : 4;
            uint32_t protected_0_en : 1;
            uint32_t protected_1_en : 1;
            uint32_t protected_2_en : 1;
            uint32_t protected_3_en : 1;
            uint32_t drv_0_en : 1;
            uint32_t drv_1_en : 1;
            uint32_t drv_2_en : 1;
            uint32_t drv_3_en : 1;
        } field;
        uint32_t val;
    } servo_controller_reg_CR;

    typedef union
    {
        struct
        {
            uint32_t realtime_err : 1;
            uint32_t mea_trig : 1;
            uint32_t adc_valid : 1;
            uint32_t drv_0_stop : 1;
            uint32_t drv_1_stop : 1;
            uint32_t drv_2_stop : 1;
            uint32_t drv_3_stop : 1;
            uint32_t drv_0_fault : 1;
            uint32_t drv_1_fault : 1;
            uint32_t drv_2_fault : 1;
            uint32_t drv_3_fault : 1;
        } field;
        uint32_t val;
    } servo_controller_reg_FLAG;

    typedef union
    {
        struct
        {
            uint32_t realtime_err : 1;
            uint32_t mea_trig : 1;
            uint32_t adc_valid : 1;
            uint32_t drv_0_stop : 1;
            uint32_t drv_1_stop : 1;
            uint32_t drv_2_stop : 1;
            uint32_t drv_3_stop : 1;
            uint32_t drv_0_fault : 1;
            uint32_t drv_1_fault : 1;
            uint32_t drv_2_fault : 1;
            uint32_t drv_3_fault : 1;
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

        volatile uint32_t spi_prescale;
        volatile uint32_t pwm_prescale;
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