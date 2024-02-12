
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <eembc_adaptation.h> // provides parse_args_from_stdin_csv, th_exit

// uart_gpio can communicate with another microcontroller through function like: 
// uart_gpio_puts, uart_gpio_can_send, uart_gpio_data_available, uart_gpio_getchar, uart_gpio_putchar, uart_gpio_printf, uart_gpio_scanf
// in the past it was used for communicating with Esp32 display for ECG project.
// Now it can be used to communicate with Esp32 display that acts as a login screen.
#include <uart_gpio.h>     
#include <sensors.h>       // analog_inputs, digital_inputs
#include <utils_flute.h>   // wait_ms, wait_s, CLK_SPEED, get_ticks_count, get_overlay_ticks_count, get_random_number
#include <stdbool.h>

void main(void) {

    int c;
    // receive the number of function calls through console input
    while ((c = getchar()) != 'q') {
        if (c == '\n' || c == '\r')
            continue;
        printf("%-10s | %-10s\n", "Analog", "Digital");
        for (int i = 0; i < 16; i++) {
            printf("%-10d | %-10d\n", analog_sensors[i], digital_sensors[i]);
        }
    }
    return 0;
}