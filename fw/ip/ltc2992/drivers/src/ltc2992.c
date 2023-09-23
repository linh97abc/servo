/*!
 LTC2992: Dual Wide Range Power Monitor

@verbatim

The LTC®2992 is a rail-to-rail system monitor that measures
current, voltage, power, charge and energy. It features an
operating range of 2.7V to 100V and includes a shunt regulator
for supplies above 100V. The current measurement common mode
range of 0V to 100V is independent of the input supply.
A 12-bit ADC measures load current, input voltage and an
auxiliary external voltage. Load current and internally
calculated power are integrated over an external clock or
crystal or internal oscillator time base for charge and energy.
An accurate time base allows the LTC2992 to provide measurement
accuracy of better than ±0.6% for charge and ±1% for power and
energy. Minimum and maximum values are stored and an overrange
alert with programmable thresholds minimizes the need for software
polling. Data is reported via a standard I2C interface.
Shutdown mode reduces power consumption to 15uA.


I2C DATA FORMAT (MSB FIRST):

Data Out:
Byte #1                                    Byte #2                     Byte #3

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  X  X C5 C4 C3 C2 C1 C0 SACK D7 D6 D5 D4 D3 D2 D1 D0 SACK  STOP

Data In:
Byte #1                                    Byte #2                                    Byte #3

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  X  X  C5 C4 C3 C2 C1 C0 SACK  Repeat Start SA6 SA5 SA4 SA3 SA2 SA1 SA0 R SACK

Byte #4                                   Byte #5
MSB                                       LSB
D15 D14  D13  D12  D11  D10  D9 D8 MACK   D7 D6 D5 D4 D3  D2  D1  D0  MNACK  STOP

START       : I2C Start
Repeat Start: I2c Repeat Start
STOP        : I2C Stop
SAx         : I2C Address
SACK        : I2C Slave Generated Acknowledge (Active Low)
MACK        : I2C Master Generated Acknowledge (Active Low)
MNACK       : I2c Master Generated Not Acknowledge
W           : I2C Write (0)
R           : I2C Read  (1)
Cx          : Command Code
Dx          : Data Bits
X           : Don't care



Example Code:

Read power, current and voltage

    CTRLA = LTC2992_CHANNEL_CONFIG_V_C_3|LTC2992_SENSE_PLUS|LTC2992_OFFSET_CAL_EVERY|LTC2992_ADIN_GND;  //! Set Control A register to default value in continuous mode
    ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_CTRLA_REG, CTRLA);   //! Sets the LTC2992 to continuous mode

    resistor = .02; // Resistor Value On Demo Board

    ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_POWER_MSB2_REG, &power_code);  // Reads the ADC registers that contains V^2
    power = LTC2992_code_to_power(power_code, resistor, LTC2992_Power_lsb); // Calculates power from power code, resistor value and power lsb

    ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_DELTA_SENSE_MSB_REG, &current_code); // Reads the voltage code across sense resistor
    current = LTC2992_code_to_current(current_code, resistor, LTC2992_DELTA_SENSE_lsb); // Calculates current from current code, resistor value and current lsb

    ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_SENSE_MSB_REG, &SENSE_code);   // Reads SENSE voltage code
    SENSE = LTC2992_SENSE_code_to_voltage(SENSE_code, LTC2992_SENSE_lsb);  // Calculates SENSE voltage from SENSE code and lsb

    ack |= LTC2992_read_32_bits(LTC2992_I2C_ADDRESS, LTC2992_ENERGY_MSB3_REG, &energy_code);  // Reads energy code
  energy = LTC2992_code_to_energy(energy_code,resistor,LTC2992_Power_lsb, LTC2992_INTERNAL_TIME_lsb); //Calculates Energy in Joules from energy_code, resistor, power lsb and time lsb

  ack |= LTC2992_read_32_bits(LTC2992_I2C_ADDRESS, LTC2992_CHARGE_MSB3_REG, &charge_code);  // Reads charge code
    charge = LTC2992_code_to_coulombs(charge_code,resistor,LTC2992_DELTA_SENSE_lsb, LTC2992_INTERNAL_TIME_lsb); //Calculates charge in coulombs from charge_code, resistor, current lsb and time lsb



@endverbatim

http://www.linear.com/product/LTC2992

http://www.linear.com/product/LTC2992#demoboards


Copyright 2018(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
 - Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.
 - The use of this software may or may not infringe the patent rights
   of one or more patent holders.  This license does not release you
   from the requirement that you obtain separate licenses from these
   patent holders to use this software.
 - Use of the software either in source or binary form, must be run
   on or directly connected to an Analog Devices Inc. component.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//! @ingroup Power_Monitors
//! @{
//! @defgroup LTC2992 LTC2992: Dual Wide Range Power Monitor
//! @}
/*! @file
    @ingroup LTC2992
    Header for LTC2992: Dual Wide Range Power Monitor
*/

#include <stdint.h>
#include "ltc2992.h"
#include "avl_ltc2992.h"
#include <io.h>
#include <sys/alt_debug.h>
#include <string.h>

#define LTC2992_IOWR(dev, addr, data) IOWR((uintptr_t)dev->BASE, addr, data)
#define LTC2992_IORD(dev, addr) IORD((uintptr_t)dev->BASE, addr)

static int i2c_write_block(LTC2992_Dev *dev,
                           uint8_t i2c_address,
                           uint8_t adc_command,
                           const void *code,
                           uint8_t len)
{
    ALT_DEBUG_ASSERT((len <= sizeof(uint32_t)));
    uint32_t stt;
    INT8U err;
    uint32_t data;
    avl_ltc2992_CR_REG crVal;
    crVal.field.command = adc_command;
    crVal.field.i2c_addr = i2c_address;
    crVal.field.data_len = len;
    crVal.field.rw = 0;
    crVal.field.en = 1;

    memcpy(&data, code, len);

    OSFlagPend(dev->flag,
               LTC2992_FLAG_LOCK,
               OS_FLAG_WAIT_CLR_ANY + OS_FLAG_CONSUME,
               0,
               &err);
    ALT_DEBUG_ASSERT((err == OS_ERR_NONE));
    LTC2992_IOWR(dev, LTC2992_CR_REG, crVal.data);
    LTC2992_IOWR(dev, LTC2992_TX_REG, data);
    LTC2992_IOWR(dev, LTC2992_CR_REG, crVal.data | LTC2992_CR_TRANS);
    OSFlagPend(dev->flag,
               LTC2992_FLAG_DONE,
               OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
               0,
               &err);
    ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

    stt = LTC2992_IORD(dev, LTC2992_FLAG_REG);
    OSFlagPost(dev->flag,
               LTC2992_FLAG_LOCK,
               OS_FLAG_CLR,
               &err);
    ALT_DEBUG_ASSERT((err == OS_ERR_NONE));
    //

    return (stt & LTC2992_FLAG_I2C_NACK) ? 1 : 0;
}

static int i2c_read_block(LTC2992_Dev *dev,
                          uint8_t i2c_address,
                          uint8_t adc_command,
                          void *code,
                          uint8_t len)
{
    ALT_DEBUG_ASSERT((len <= sizeof(uint32_t)));
    uint32_t stt;
    INT8U err;
    uint32_t data;
    avl_ltc2992_CR_REG crVal;
    crVal.field.command = adc_command;
    crVal.field.i2c_addr = i2c_address;
    crVal.field.data_len = len;
    crVal.field.rw = 1;
    crVal.field.en = 1;

    OSFlagPend(dev->flag,
               LTC2992_FLAG_LOCK,
               OS_FLAG_WAIT_CLR_ANY + OS_FLAG_CONSUME,
               0,
               &err);
    ALT_DEBUG_ASSERT((err == OS_ERR_NONE));
    LTC2992_IOWR(dev, LTC2992_CR_REG, crVal.data);
    LTC2992_IOWR(dev, LTC2992_CR_REG, crVal.data | LTC2992_CR_TRANS);
    OSFlagPend(dev->flag,
               LTC2992_FLAG_DONE,
               OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
               0,
               &err);
    ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

    data = LTC2992_IORD(dev, LTC2992_RX_REG);
    stt = LTC2992_IORD(dev, LTC2992_FLAG_REG);
    OSFlagPost(dev->flag,
               LTC2992_FLAG_LOCK,
               OS_FLAG_CLR,
               &err);
    ALT_DEBUG_ASSERT((err == OS_ERR_NONE));
    //

    if (stt & LTC2992_FLAG_I2C_NACK)
    {
        return 1;
    }

    memcpy(code, &data, len);

    return 0;
}

// Write an 8-bit code to the LTC2992.
int8_t LTC2992_write(LTC2992_Dev *dev,
                     uint8_t i2c_address, uint8_t adc_command, uint8_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
    return i2c_write_block(dev,
                           i2c_address,
                           adc_command,
                           &code,
                           1);
}

// Write a 16-bit code to the LTC2992.
int8_t LTC2992_write_16_bits(LTC2992_Dev *dev, uint8_t i2c_address, uint8_t adc_command, uint16_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
    uint8_t data[2];
    data[0] = code >> 8;
    data[1] = code & 0xFFu;

    return i2c_write_block(dev,
                           i2c_address,
                           adc_command,
                           data,
                           sizeof(data));
}

// Write a 24-bit code to the LTC2992.
int8_t LTC2992_write_24_bits(LTC2992_Dev *dev, uint8_t i2c_address, uint8_t adc_command, uint32_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
    uint8_t data[3];
    data[0] = (code >> 16) & 0xFFu;
    data[0] = (code >> 8) & 0xFFu;
    data[1] = code & 0xFFu;

    return i2c_write_block(dev,
                           i2c_address,
                           adc_command,
                           data,
                           sizeof(data));
}

// Reads an 8-bit adc_code from LTC2992
int8_t LTC2992_read(LTC2992_Dev *dev, uint8_t i2c_address, uint8_t adc_command, uint8_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
    return i2c_read_block(dev, i2c_address, adc_command, adc_code, 1);
}

// Reads a 12-bit adc_code from LTC2992
int8_t LTC2992_read_12_bits(LTC2992_Dev *dev, uint8_t i2c_address, uint8_t adc_command, uint16_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
    // Use union type defined in Linduino.h to combine two uint8_t's (8-bit unsigned integers) into one uint16_t (unsigned 16-bit integer)
    // Then, shift by 4 bits and return in *adc_code
    int32_t ack;
    uint8_t data[2];

    ack = i2c_read_block(dev, i2c_address, adc_command, data, 2);
    if (ack)
    {
        return ack;
    }

    *adc_code = ((uint16_t)data[0] << 4) | (data[1] >> 4);

    return 0;
}

// Reads a 16-bit adc_code from LTC2992
int8_t LTC2992_read_16_bits(LTC2992_Dev *dev, uint8_t i2c_address, uint8_t adc_command, uint16_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
    int32_t ack;
    uint8_t data[2];
    ack = i2c_read_block(dev, i2c_address, adc_command, data, 2);

    if (ack)
    {
        return ack;
    }

    *adc_code = ((uint16_t)data[0] << 8) | data[1];

    return 0;
}

// Reads a 24-bit adc_code from LTC2992
int8_t LTC2992_read_24_bits(LTC2992_Dev *dev, uint8_t i2c_address, uint8_t adc_command, uint32_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
    int8_t ack;
    uint8_t data[3];
    ack = i2c_read_block(dev, i2c_address, adc_command, data, 3);
    ack = i2c_read_block(dev, i2c_address, adc_command, data, 2);

    if (ack)
    {
        return ack;
    }

    *adc_code = ((uint32_t)data[0] << 16) |
                ((uint32_t)data[1] << 8) |
                data[2];

    return 0;
}

// Calculate the LTC2992 SENSE voltage
float LTC2992_SENSE_code_to_voltage(uint16_t adc_code, float LTC2992_SENSE_lsb)
// Returns the SENSE Voltage in Volts
{
    float voltage;
    voltage = (float)adc_code * LTC2992_SENSE_lsb; //! 1) Calculate voltage from code and lsb
    return (voltage);
}

// Calculate the LTC2992 GPIO voltage
float LTC2992_GPIO_code_to_voltage(uint16_t adc_code, float LTC2992_GPIO_lsb)
// Returns the GPIO Voltage in Volts
{
    float adc_voltage;
    adc_voltage = (float)adc_code * LTC2992_GPIO_lsb; //! 1) Calculate voltage from code and ADIN lsb
    return (adc_voltage);
}

// Calculate the LTC2992 current with a sense resistor
float LTC2992_code_to_current(uint16_t adc_code, float resistor, float LTC2992_DELTA_SENSE_lsb)
// Returns the LTC2992 current in Amps
{
    float voltage, current;
    voltage = (float)adc_code * LTC2992_DELTA_SENSE_lsb; //! 1) Calculate voltage from ADC code and delta sense lsb
    current = voltage / resistor;                        //! 2) Calculate current, I = V/R
    return (current);
}

// Calculate the LTC2992 current with a sense resistor
float LTC2992_code_to_current_sum(uint16_t adc_code, float resistor, float LTC2992_DELTA_SENSE_lsb)
// Returns the LTC2992 current in Amps
{
    float voltage, current;
    voltage = (float)(adc_code << 1) * LTC2992_DELTA_SENSE_lsb; //! 1) Calculate voltage from ADC code and delta sense lsb
    current = voltage / resistor;                               //! 2) Calculate current, I = V/R
    return (current);
}

// Calculate the LTC2992 power
float LTC2992_code_to_power(int32_t adc_code, float resistor, float LTC2992_Power_lsb)
// Returns The LTC2992 power in Watts
{
    float power;
    power = (float)adc_code * LTC2992_Power_lsb / resistor; //! 1) Calculate Power using Power lsb and resistor

    return (power);
}

// Calculate the LTC2992 power
float LTC2992_code_to_power_sum(int32_t adc_code, float resistor, float LTC2992_Power_lsb)
// Returns The LTC2992 power in Watts
{
    float power;
    power = (float)(adc_code << 1) * LTC2992_Power_lsb / resistor; //! 1) Calculate Power using Power lsb and resistor

    return (power);
}
