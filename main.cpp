#include "mbed.h"

RawSerial pc(USBTX, USBRX);
RawSerial dev(WIFI_CELLULAR_TX, WIFI_CELLULAR_RX);

DigitalOut wifi_prog_no(PROG_WIFI_N, 0); // high flash boot, low uart download mode
                                         // change to 1 in order to test AT commands
                                         // make sure to match dev baudrate below since
                                         // in normal mode the ESP comes up at 115200
DigitalOut wifi_reset_n(RESET_N, 1);
DigitalOut wifi_no(WIFI_N, 1);
DigitalOut wifi_power_enable(WIFI_PWR_EN, 1);
DigitalOut cell_power_control(CELL_PWR_EN, 0);
DigitalOut gnss_power_control(GPS_PWR_EN, 0);
DigitalOut status_led(STATUS_LED); // LED1 is mapped to STATUS_LED

DigitalOut led1(LED1);
DigitalOut led4(LED1);

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

int main()
{
    pc.baud(115200);
    dev.baud(115200);

    pc.attach(&pc_recv, Serial::RxIrq);
    dev.attach(&dev_recv, Serial::RxIrq);

    pc.printf("TARGET_MM> Ready\r\n");
    wifi_power_enable.write(1);

    while (true)
    {
        sleep();
    }

    return (0);
}
