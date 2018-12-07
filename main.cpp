#include "mbed.h"
#include "max77801.h"

RawSerial pc(USBTX, USBRX, 115200);
RawSerial dev(WIFI_CELLULAR_TX, WIFI_CELLULAR_RX, 115200);

DigitalOut wifi_reset_n(RESET_N, 1);           // reset line high to deassert
DigitalOut wifi_no(WIFI_N, 0);                 // configure mux for wifi
DigitalOut cell_power_control(CELL_PWR_EN, 1); // turn off cell
DigitalOut cell_on(CELL_ON, 1);                // turn off cell
DigitalOut gnss_power_control(GPS_PWR_EN, 0);  // turn off gps
DigitalOut wifi_power_enable(WIFI_PWR_EN, 0);  // vcc power on

DigitalOut led1(LED1);
DigitalOut led4(LED1);

AnalogIn cell_1v8(CELL_1v8);
AnalogIn cell_3v8(CELL_3v8);
AnalogIn cell_vcore_1v1(CELL_VCORE_1v1);

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

int init_cellular_power(void)
{
    wifi_power_enable.write(0);
    cell_power_control.write(1);
    I2C i2cBus(DCDC_I2C_SDA, DCDC_I2C_SCL);
    i2cBus.frequency(400000);

    MAX77801 max77801(&i2cBus);
    wait_ms(100);
    wait(1);

    int rData = max77801.init();
    if (rData < 0)
    {
        printf("MAX77801 Fail to Init. Stopped\r\n");
        return -1;
    }
    else
    {
        printf("MAX77801 Init Done\r\n");
    }

    // write 3.8v
    rData = max77801.write_register(MAX77801::REG_VOUT_DVS_L, 0x60);
    if (rData < 0)
    {
        printf("Error: to access data\r\n");
    }

    pc.printf("cell_vcore_1v1 (%f), cell_1v8 (%f), cell_3v8 (%f)\r\n", cell_vcore_1v1.read(), cell_1v8.read(), cell_3v8.read());
}

int main()
{
    init_cellular_power();

    pc.printf("TARGET_MM> Ready\r\n");
    pc.attach(&pc_recv, Serial::RxIrq);
    dev.attach(&dev_recv, Serial::RxIrq);

    while (true)
    {
        sleep();
    }

    return (0);
}
