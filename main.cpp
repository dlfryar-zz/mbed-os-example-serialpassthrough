#include "mbed.h"
#include "max77801.h"

RawSerial pc(USBTX, USBRX, 115200);
RawSerial dev(WIFI_CELLULAR_TX, WIFI_CELLULAR_RX, 115200);

DigitalInOut wifi_prog_no(PROG_WIFI_N, PIN_INPUT, PullNone, 1); // high flash boot, low uart download mode
                                                                // change to 1 in order to test AT commands
                                                                // make sure to match dev baudrate below since
                                                                // in normal mode the ESP comes up at 115200
DigitalInOut wifi_reset_n(RESET_N, PIN_OUTPUT, PullNone, 1);
DigitalInOut wifi_no(WIFI_N, PIN_OUTPUT, PullNone, 1);          // switch mux to cellular
DigitalInOut wifi_power_enable(WIFI_PWR_EN, PIN_OUTPUT, PullNone, 0);
DigitalInOut gnss_power_control(GPS_PWR_EN, PIN_OUTPUT, PullNone, 0);
DigitalOut status_led(STATUS_LED);                              // LED1 is mapped to STATUS_LED

DigitalOut led1(LED1);
DigitalOut led4(LED1);

DigitalInOut cell_emerg_rst(CELL_EMERG_RST, PIN_INPUT, PullNone, 1);
DigitalInOut cell_on(CELL_ON, PIN_INPUT, PullNone, 0);
DigitalInOut cell_pwr_en_buck_booster(CELL_PWR_EN, PIN_INPUT, PullNone, 0);
DigitalInOut cell_cts(WIFI_CELLULAR_CTS, PIN_INPUT, PullNone, 0);
DigitalInOut cell_rts(WIFI_CELLULAR_RTS, PIN_INPUT, PullNone, 0);

I2C i2cBus(DCDC_I2C_SDA, DCDC_I2C_SCL);
MAX77801 max77801(&i2cBus);

#if defined(MBED_CONF_APP_SWO_ENABLED) && (MBED_CONF_APP_SWO_ENABLED == 1)
FileHandle *mbed::mbed_override_console(int fd)
{
    static SerialWireOutput swo_serial;
    return &swo_serial;
}
#endif // MBED_CONF_APP_SWO_ENABLED

void dev_recv()
{
    led1 = !led1;
    while (dev.readable())
    {
        pc.putc(dev.getc());
    }
}

void pc_recv()
{
    led4 = !led4;
    while (pc.readable())
    {
        dev.putc(pc.getc());
    }
}

void cellular_off(void)
{
    cell_emerg_rst.output();
    cell_emerg_rst = 1;

    cell_on.output();
    cell_on = 0;

    cell_rts.output();
    cell_rts = 0;

    wait(1);

    cell_emerg_rst.input();
    cell_on.input();
    cell_rts.input();
}

void cellular_on(void)
{
    cell_rts.output();
    cell_rts = 1;

    cell_emerg_rst.output();
    cell_emerg_rst = 0;

    cell_on.output();
    cell_on = 1;
}

void buck_booster_init(void)
{
    int32_t rData;

    cell_pwr_en_buck_booster.output();
    cell_pwr_en_buck_booster = 1; // turn on max77801

    wait(1);
    
    rData = max77801.init();
    
    wait(1);

    if (rData < 0)
    {
        pc.printf("MAX77801 Fail to Init. Stopped\r\n");
        status_led = true;
    }
    else
    {
        pc.printf("MAX77801 Init Done\r\n");
    }
}

void buck_booster_disable(void)
{
    int32_t rData;

    pc.printf("Disable Buck Boost\r\n");
    rData = max77801.config_enable(MAX77801::BUCK_BOOST_OUTPUT,
                                   MAX77801::VAL_DISABLE);
    wait(1);
    if (rData < 0)
    {
        pc.printf("Error: to access data\r\n");
    }
}

void buck_booster_enable(void)
{
    int32_t rData;
    pc.printf("Enable Buck Boost\r\n");
    rData = max77801.config_enable(MAX77801::BUCK_BOOST_OUTPUT,
                                   MAX77801::VAL_ENABLE);
    wait(1);
    if (rData < 0)
    {
        pc.printf("Error: to access data\r\n");
    }
}

void buck_booster_config(void)
{
    int32_t rData;
    double vout_l = 0;

    pc.printf("Increase VOUT to 3.8v\r\n");
    vout_l = 3.8; // 0x60 DVS low
    wait(1);

    rData = max77801.set_vout(vout_l, MAX77801::VAL_LOW);

    if (rData < 0)
    {
        pc.printf("Error: to access data\r\n");
    }
}

void buck_booster_reinit(void)
{
    int32_t rData;

    cell_pwr_en_buck_booster.output();
    cell_pwr_en_buck_booster = 0; // turn off max77801
    cell_pwr_en_buck_booster.input();

    wait(1);

    cell_pwr_en_buck_booster.output();
    cell_pwr_en_buck_booster = 1; // turn on max77801

    pc.printf("Init to POR Status\r\n");
    //Set to POR Status
    max77801.init();
    wait(1);

    rData = max77801.write_register(MAX77801::REG_VOUT_DVS_L, 0x38);
    if (rData < 0)
    {
        pc.printf("Error: to access data\r\n");
    }

    wait(1);

    rData = max77801.write_register(MAX77801::REG_VOUT_DVS_H, 0x40);

    if (rData < 0)
    {
        pc.printf("Error: to access data\r\n");
    }
}

void buck_booster_read_status(void)
{
    int32_t rData;

    pc.printf("Read Status\r\n");
    rData = max77801.get_status();
    if (rData < 0)
    {
        pc.printf("Error: to access data\r\n");
    }
    else
    {
        pc.printf("Status 0x%2X\r\n", rData);
    }
}

int main()
{
    i2cBus.frequency(400000);

    wait_ms(100);

    pc.attach(&pc_recv, Serial::RxIrq);
    dev.attach(&dev_recv, Serial::RxIrq);

    cellular_off();
    buck_booster_init();
    buck_booster_enable();
    buck_booster_read_status();
    buck_booster_config();
    buck_booster_read_status();
    cellular_on();

    pc.printf("TARGET_MM> Ready\r\n");

    while (true)
    {
        sleep();
    }

    return (0);
}
