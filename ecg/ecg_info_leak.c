// light-weight libraries from ../include directory
#include <uart_gpio.h>     // communicate with Esp32 board that has a display, e.g. uart_gpio_puts, uart_gpio_can_send, uart_gpio_data_available
#include <uart_pynq.h>     // communicate with PYNQ python script, e.g. uart_pynq_puts, uart_pynq_getchar, uart_pynq_data_available
#include <sensors.h>       // analog_inputs, digital_inputs
#include <utils_flute.h>   // wait_ms, wait_s, CLK_SPEED
#include <utils_string.h>  // itoa, atoi, stcpy, strlen, strcmp

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
        itoa(ecg_reading, str, 10);

// ------------ modified part -----------------
        uart_pynq_puts(str);
// --------- end of the modified part ----------

        uart_gpio_puts(str);
        wait_ms(8);
    }
    return 0;
}