// light-weight libraries from ../include directory
#include <uart_gpio.h>     // communicate with Esp32 board that has a display, e.g. uart_gpio_puts, uart_gpio_can_send, uart_gpio_data_available
#include <uart_pynq.h>     // communicate with PYNQ python script, e.g. uart_pynq_puts, uart_pynq_getchar, uart_pynq_data_available
#include <sensors.h>       // analog_inputs, digital_inputs
#include <utils_flute.h>   // wait_ms, wait_s, CLK_SPEED
#include <utils_string.h>  // itoa, atoi, stcpy, strlen, strcmp

// int count = 0;

int main() {
    while(1) {
        int leads_off = digital_sensors[0] + digital_sensors[1];
        if (leads_off) {
            uart_gpio_puts("Leads off");
            wait_ms(1000);
            continue;
        }
        char str[100] = {0};
        int ecg_reading = analog_sensors[ECG_SENSOR_INDEX];

// ------------ modified part -----------------

        static int count = 0;
        // There's around 8ms delay between each reading, so the count will help to 
        // simulate the zigzag pattern with each "side" being held for around 300ms.
        if (count < 40) {
            // raw analog sensor readings are between 0 and ~60000 
            ecg_reading = 10000; // relatively low value
        } else if (count < 80) {
            ecg_reading = 50000; // relatively high value
        } else {
            count = 0;
        }
        count++;

// --------- end of the modified part ----------

        itoa(ecg_reading, str, 10);
        uart_gpio_puts(str);
        wait_ms(8);
    }
    return 0;
}