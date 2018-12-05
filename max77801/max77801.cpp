/*******************************************************************************
 * Copyright (C) 2017 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *******************************************************************************
 */
 #include "max77801.h"
 
/***** Definitions *****/
#define I2C_ADDR            (0x18<<1)
 
/**
  * MAX77801 constructor.
  *
  * @param i2c I2C object to use.
  */
MAX77801::MAX77801(I2C *i2c) :
    i2c_(i2c)
{
    i2c_owner = false;
}
 
/**
  * MAX77801 destructor.
  */
MAX77801::~MAX77801()
{
    if(i2c_owner) {
        delete i2c_;
    }
}

/**
  * @brief   Initialize MAX77801
  */
int32_t MAX77801::init()
{ 
    int32_t data;
    
    data = write_register(REG_CONFIG1, 0x0E);
    if(data < 0)
        return -1;
    
    data = write_register(REG_CONFIG2, 0x70);
    if(data < 0)
        return -1;
        
    return 0;
}
 
/**
  * @brief   Read Register
  * @details Reads data from MAX77801 register
  *
  * @param   reg_addr Register to read
  * @returns data if no errors, -1 if error.
  */
int32_t MAX77801::read_register(MAX77801::registers_t reg_no)
{
    char data;
 
    data = reg_no;
    if (i2c_->write(I2C_ADDR, &data, 1, true) != 0) {
        return -1;
    }
 
    if (i2c_->read(I2C_ADDR | 0x01, &data, 1) != 0) {
        return -1;
    }
 
    return (0x0 + data);
}
 
/**
  * @brief   Write Register
  * @details Writes data to MAX77756 register
  *
  * @param   reg_addr Register to write
  * @param   reg_data Data to write
  * @returns 0 if no errors, -1 if error.
  */
int32_t MAX77801::write_register(MAX77801::registers_t reg_no, char reg_data)
{
    char data[2];
 
    data[0] = reg_no;
    data[1] = reg_data;
    if (i2c_->write(I2C_ADDR, data, 2) != 0) {
        return -1;
    }
 
    return 0;
}

/**
  * @brief   Update Register data
  * @details Update bits data of a register 
  *
  * @param   reg_no Register Number to be updated
  * @param   mask Mask Data
  * @param   reg_data bit data
  * @returns 0 if no errors, -1 if error.
  */
int32_t MAX77801::update_register
(MAX77801::registers_t reg_no, char reg_mask,  char reg_data)
{
    int32_t data;
 
    data = read_register(reg_no);
    if(data < 0)
        return -1;
    
    data &= ~reg_mask;
    data |= reg_data;
    
    data = write_register(reg_no, (char)(data & 0xff));
    if(data < 0)
        return -1;
    return 0;
}

/**
  * @brief   Get version info
  * @details 0 : Plain
  *
  * @param   None
  * @returns version info.
  */
char*  MAX77801::get_version()
{
    int32_t data;
    
    data = read_register(REG_DEVICE_ID);
    switch ((data >> 3) & 0xf)
    {
        case 0x0: 
            return "PLAIN";
        case 0x1: 
            return "-1Z";
        case 0x2:
            return "-2Z";        
    }
    return "UNKNOWN";
}

/**
  * @brief   Get revision info
  * @details 0x1 : PASS1
  *          0x2 : PASS2  
  *          0x3 : PASS3
  * @param   None
  * @returns revision info.
  */
char*  MAX77801::get_revision()
{
    int32_t data;
    
    data = read_register(REG_DEVICE_ID);
    switch(data & 0x7)
    {
        case 0x0: 
            return "PASS1";
        case 0x1: 
            return "PASS2";
        case 0x2:
            return "PASS3";        
    }
    return "UNKNOWN";    
}

/**
  * @brief   Get status
  * @details Get status register data
  *          BIT3 : Junction Temperature info
  *          BIT2 : Buck Boost POK Status
  *          BIT1 : Buck Boost OVP Status
  *          BIT0 : Buck Boost OCP Status
  * @param   None
  * @returns status register data.
  */
int32_t MAX77801::get_status()
{    
    int32_t data;
    
    data = read_register(REG_DEVICE_ID);
    if(data < 0)
        return -1;
    return (data & 0x0f);
}

/**
  * @brief   config enable bit
  * @details Set a Config bit controlled using enabled/disabled
  * @param   config : config bit
  * @param   en : enable/disable
  * @returns 0 if no errors, -1 if error.
  */
int32_t MAX77801::config_enable(MAX77801::config_enabled_t config,
                                    MAX77801::enable_t en)
{    
    int32_t data;
    
    switch(config)
    {
        case ACTIVE_DISCHARGE:
            data = update_register(REG_CONFIG1, 0x02, ((char)en) <<1);
        break;
        case FORCED_PWM:
            data = update_register(REG_CONFIG1, 0x01, ((char)en) <<0);
        break;
        case BUCK_BOOST_OUTPUT:
            data = update_register(REG_CONFIG2, 0x40, ((char)en) <<6);
        break;
        case EN_PULL_DOWN:
            data = update_register(REG_CONFIG2, 0x20, ((char)en) <<5);
        break;
        default:
            return -1;
    }
    
    if(data < 0)
        return -1;
    return 0;
}

/**
  * @brief   Config Ramp Up
  * @details Set BB_RU_SR
  * 
  * @param   config : config value
  * @returns 0 if no errors, -1 if error.
  */           
int32_t MAX77801::config_ramp_up(MAX77801::ramp_up_rate_t config)
{
    int32_t data;
    
    data = update_register(REG_CONFIG1, 0x20, ((char)config) <<5);
    if(data < 0)
        return -1;
    return 0;
}

/**
  * @brief   Config Ramp Down
  * @details Set BB_RD_SR
  * 
  * @param   config : config value
  * @returns 0 if no errors, -1 if error.
  */           
int32_t MAX77801::config_ramp_down(MAX77801::ramp_dn_rate_t config)
{
    int32_t data;
    
    data = update_register(REG_CONFIG1, 0x10, ((char)config) <<4);
    if(data < 0)
        return -1;
    return 0;
}

/**
  * @brief   Config OVP Threshold
  * @details Set BB_OVP_TH bits
  * 
  * @param   config : config value
  * @returns 0 if no errors, -1 if error.
  */           
int32_t MAX77801::config_ovp_threshold(MAX77801::output_ovp_threshold_t config)
{    
    int32_t data;
    
    data = update_register(REG_CONFIG1, 0x0C, ((char)config) <<2);
    if(data < 0)
        return -1;
    return 0;
}

/**
  * @brief   config pokpol active bit
  * @details Set POK_POL Bit
  * 
  * @param   lowHigh : active value
  * @returns 0 if no errors, -1 if error.
  */           
int32_t MAX77801::config_pokpol_active(MAX77801::low_high_t lowHigh)
{
    int32_t data;
    
    data = update_register(REG_CONFIG2, 0x10, ((char)lowHigh) <<4);
    if(data < 0)
        return -1;
    return 0;
}

/**
  * @brief   Set VOUT Voltage when DVS = Low
  * @details Set Vout Voltage
  *
  * @param   vout level from 2.6000V ~ 4.1875V with 0.0125V step
  * @returns 0 if no errors, -1 if error.
  */
  
int32_t MAX77801::set_vout(double level, MAX77801::low_high_t dvs)
{
    int32_t ret_val = 0;
    char reg_data = 0;
    
    if(level < 2.6000 || level > 4.1875)
        return -1;
    
    reg_data = (char)((level-2.6000)*80);
    
    if(dvs == VAL_LOW)
        ret_val = write_register(REG_VOUT_DVS_L,reg_data);
    else
        ret_val = write_register(REG_VOUT_DVS_H,reg_data);
    
    if(ret_val < 0)
        return -1;
    return 0;
}

