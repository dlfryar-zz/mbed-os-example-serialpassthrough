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
#ifndef _MAX77801_H_
#define _MAX77801_H_
 
#include "mbed.h"

class MAX77801
{
 
public:

    /**
     * @brief   Register Addresses
     * @details Enumerated MAX77801 register addresses
     */
    typedef enum {
        REG_DEVICE_ID = 0x00,
        REG_STATUS,
        REG_CONFIG1,
        REG_CONFIG2,
        REG_VOUT_DVS_L,
        REG_VOUT_DVS_H 
    } registers_t;
   
    /**
     * @brief   ENABLE/DISABLE
     * @details Enumerated ENABLE/DISABLE
     */
    typedef enum {
        VAL_DISABLE = 0x00,
        VAL_ENABLE
    } enable_t;
   
    /**
     * @brief   Config Enabled
     * @details Enumerated Configs set by ENABLE/DISABLE
     */
    typedef enum {
        ACTIVE_DISCHARGE = 0x00,
        FORCED_PWM,
        BUCK_BOOST_OUTPUT,
        EN_PULL_DOWN
    } config_enabled_t;
   
    /**
     * @brief   LOW/HIGH
     * @details Enumerated LOW/HIGH
     */
    typedef enum {
        VAL_LOW = 0x00,
        VAL_HIGH
    } low_high_t;
    
    /**
     * @brief   POLLING ACTIVE LEVEL
     * @details Enumerated POLLING ACTIVE LEVEL
     */     
    typedef enum {
        POLL_ACTIVE_LOW = 0x00,
        POLL_ACTIVE_HIGH
    } poll_level_t;
    
    /**
     * @brief   Buck Boost Ramp-Up Slew Rate
     * @details Enumerated Buck Boost Rising-Up Slew Rate
     */     
    typedef enum {
        RU_SR_12P5_MV_PER_US = 0x00,
        RU_SR_25P0_MV_PER_US
    } ramp_up_rate_t;
    
    /**
     * @brief   Buck Boost Ramp-Down Slew Rate
     * @details Enumerated Buck Boost Rising-Down Slew Rate
     */     
    typedef enum {
        RD_SR_3P125_MV_PER_US = 0x00,
        RD_SR_6P250_MV_PER_US
    } ramp_dn_rate_t;
    
    /**
     * @brief   OUTPUT OVP Threshold
     * @details Enumerated OUTPUT OVP Threshold
     */     
    typedef enum {
        OUTPUT_THRESH_NO_OVP = 0x00,
        OUTPUT_THRESH_110_PERCENT_OVP,
        OUTPUT_THRESH_115_PERCENT_OVP,
        OUTPUT_THRESH_120_PERCENT_OVP,
    } output_ovp_threshold_t;
        
    /**
      * MAX77801 constructor.
      *
      * @param i2c I2C object to use.
      */
    MAX77801(I2C *i2c);
 
    /**
      * MAX77801 destructor.
      */
    ~MAX77801();
  
    /**
      * @brief   Initialize MAX77801
      */
    int32_t init();
 
    /**
      * @brief   Write Register
      * @details Writes data to MAX77801 register
      *
      * @param   reg_addr Register to write
      * @param   reg_data Data to write
      * @returns 0 if no errors, -1 if error.
      */
    int32_t write_register(MAX77801::registers_t reg_addr, char reg_data);
 
    /**
      * @brief   Read Register
      * @details Reads data from MAX77801 register
      *
      * @param   reg_addr Register to read
      * @returns data if no errors, -1 if error.
      */
    int32_t read_register(MAX77801::registers_t reg_addr);
    
    /**
      * @brief   Update Register data
      * @details Update bits data of a register 
      *
      * @param   reg_no Register Number to be updated
      * @param   mask Mask Data
      * @param   reg_data bit data
      * @returns 0 if no errors, -1 if error.
      */
    int32_t update_register
    (MAX77801::registers_t reg_no, char reg_mask,  char reg_data);
       
    /**
     * @brief   Get version info
     * @details 0 : Plain
     *
     * @param   None
     * @returns version info.
     */
    char* get_version();   
    /**
     * @brief   Get revision info
     * @details 0x1 : PASS1
     *          0x2 : PASS2  
     *          0x3 : PASS3
     * @param   None
     * @returns revision info.
     */
    char* get_revision();

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
    int32_t get_status();

    /**
      * @brief   config enable bit
      * @details Set a Config bit controlled using enabled/disabled
      * @param   config : config bit
      * @param   en : enable/disable
      * @returns 0 if no errors, -1 if error.
      */
    int32_t config_enable(MAX77801::config_enabled_t config,
                          MAX77801::enable_t en);

    /**
      * @brief   Config Ramp Up
      * @details Set BB_RU_SR
      * 
      * @param   config : config value
      * @returns 0 if no errors, -1 if error.
      */           
    int32_t config_ramp_up(MAX77801::ramp_up_rate_t config);

    /**
      * @brief   Config Ramp Down
      * @details Set BB_RD_SR
      * 
      * @param   config : config value
      * @returns 0 if no errors, -1 if error.
      */           
    int32_t config_ramp_down(MAX77801::ramp_dn_rate_t config);

    /**
      * @brief   Config OVP Threshold
      * @details Set BB_OVP_TH bits
      * 
      * @param   config : config value
      * @returns 0 if no errors, -1 if error.
      */           
    int32_t config_ovp_threshold(MAX77801::output_ovp_threshold_t config);
    
    /**
      * @brief   config pokpol active bit
      * @details Set POK_POL Bit
      * 
      * @param   lowHigh : active value
      * @returns 0 if no errors, -1 if error.
      */
    int32_t config_pokpol_active(MAX77801::low_high_t lowHigh);

    /**
     * @brief   Set VOUT Voltage when DVS = Low
     * @details Set Vout Voltage
     *
     * @param   vout level from 2.6000V ~ 4.1875V with 0.0125V step
     * @returns 0 if no errors, -1 if error.
     */
    int32_t set_vout(double level, MAX77801::low_high_t dvs);

private:
 
    I2C *i2c_;
    bool i2c_owner;
 
};
#endif /* _MAX77801_H_ */