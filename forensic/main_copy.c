

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

#define CORRECT_PASSWORD "1234"


void authenticate() {
    char login_buffer[32] = {0};
    // char password_buffer[4] = {0};
    // int is_authorized2 = 0;
    char password_buffer[4] = {0};
    int is_authorized = 0;

    // printf("location of login_buffer: %p\n", login_buffer);
    // printf("location of password_buffer: %p\n", password_buffer);
    // printf("location of is_authorized: %p\n", &is_authorized);
    // printf("address difference between password_buffer and is_authorized: %d\n", (int)password_buffer - (int)&is_authorized);
    // printf("address difference between password_buffer and is_authorized2: %d\n", (int)password_buffer - (int)&is_authorized2);
    while(!is_authorized) {
        printf("Enter password:\n> ");
        // uart_gpio_puts("Enter password:");
        // scanf is vulnerable to buffer overflow, if we supply more than 4 characters, 
        // we can overwrite the is_authorized variable without knowing the correct password
        while (!uart_gpio_data_available()) {
            wait_ms(500);
        }
        uart_gpio_scanf("%s\n", password_buffer);
        printf("Password that was entered: %s\n", password_buffer);
        // uart_gpio_gets(password_buffer);
        printf("login[0]: %d, is_authorized: %d\n", login_buffer[0], is_authorized);  
        if(!strcmp(password_buffer, CORRECT_PASSWORD)) {
            is_authorized = 1;
            puts("Correct password!");
        } else {
            puts("Incorrect password!");
        }
    }
    uart_gpio_puts("Access granted!");
    puts("Access granted!");
}

void main(void) {
    // each argument is supplied as csv string in stdin
    // all arguments are input indices, allowing to run the same test multiple times
    // char *argv[MAX_PROGRAM_ARGS]; 
    // int argc = 0;
    // tokenized_inp_str may be deallocated after argv's are not needed anymore
    // char *tokenized_inp_str = parse_args_from_stdin_csv(&argc, argv);


    while (true) {
        // for ease of training using successful logins this is called in a constant loop
        authenticate(); 
        wait_ms(1000);
    }

    // while (true) {
    //     wait_ms(1000);
    //     // main operation loop
    // }

// #ifdef SINGLE_ALGORITHM
//     if (argc < 1) 
//         th_exit("ERROR: missing input index argument.\nStopping execution.\n");

//     PROG_NAME(argc, argv);
// #endif

    // free(tokenized_inp_str);
}

// int main() {
//     while(1) {
//         int leads_off = digital_sensors[0] + digital_sensors[1];
//         if (leads_off) {
//             uart_gpio_puts("Leads off");
//             wait_ms(1000);
//             continue;
//         }
//         char str[100] = {0};
//         int ecg_reading = analog_sensors[ECG_SENSOR_INDEX];
//         itoa(ecg_reading, str, 10);
//         // uart_pynq_puts(str);
//         uart_gpio_puts(str);
//         wait_ms(8);
//     }
//     return 0;
// }