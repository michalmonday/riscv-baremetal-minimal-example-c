
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

bool is_barcode_valid(long long barcode);
bool is_ean13_barcode(long long barcode);
bool is_upca_barcode(long long barcode);

void main(void) {
    // each argument is supplied as csv string in stdin
    // all arguments are input indices, allowing to run the same test multiple times
    // char *argv[MAX_PROGRAM_ARGS]; 
    // int argc = 0;
    // tokenized_inp_str may be deallocated after argv's are not needed anymore
    // char *tokenized_inp_str = parse_args_from_stdin_csv(&argc, argv);

    char cmd[32] = {0};
    char login_buffer[32] = {0};
    // char password_buffer[4] = {0};
    int is_authorized2 = 0;
    char password_buffer[4] = {0};
    int is_authorized = 0;

    printf("location of login_buffer: %p\n", login_buffer);
    printf("location of password_buffer: %p\n", password_buffer);
    printf("location of is_authorized: %p\n", &is_authorized);
    printf("address difference between password_buffer and is_authorized: %d\n", (int)password_buffer - (int)&is_authorized);
    printf("address difference between password_buffer and is_authorized2: %d\n", (int)password_buffer - (int)&is_authorized2);

LOGIN:
    while(!is_authorized) {
        printf("Enter password:\n> ");
        // uart_gpio_puts("Enter password:");
        // scanf is vulnerable to buffer overflow, if we supply more than 4 characters, 
        // we can overwrite the is_authorized variable without knowing the correct password
        while (!uart_gpio_data_available()) {
            wait_ms(500);
        }
        // let all characters arrive
        wait_ms(300);
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

    while (true) {
        while(!uart_gpio_data_available())
            wait_ms(500);
        // let all characters arrive
        wait_ms(300);
        uart_gpio_scanf("%s\n", cmd);
        if (!strcmp(cmd, "logout")) {
            puts("Logged out");
            uart_gpio_puts("Logged out");
            is_authorized = 0;
            goto LOGIN;
        }
    
        // barcode checking code
        char *end_ptr;
        long long barcode = strtoll(cmd, &end_ptr, 10);
        if (*end_ptr) {
            puts("Invalid barcode format");
            uart_gpio_puts("Invalid barcode format");
            continue;
        }

        if (is_barcode_valid(barcode)) {
            puts("Valid barcode");
            uart_gpio_puts("Valid barcode");
        } else {
            puts("Invalid barcode");
            uart_gpio_puts("Invalid barcode");
        } 
    }

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

bool is_barcode_valid(long long barcode) {
    return is_ean13_barcode(barcode) || is_upca_barcode(barcode);
}

// Function to validate EAN-13 barcode
bool is_ean13_barcode(long long barcode) {
    // Convert the long long barcode to a string for easier manipulation
    char barcodeStr[14];
    snprintf(barcodeStr, sizeof(barcodeStr), "%lld", barcode);

    // Check if the barcode is 13 digits long
    if (strlen(barcodeStr) != 13) {
        return false;
    }

    // Calculate the check digit
    int sum = 0;
    for (int i = 0; i < 12; i++) {
        int digit = barcodeStr[i] - '0';
        if (i % 2 == 0) {
            sum += digit;
        } else {
            sum += 3 * digit;
        }
    }

    int checkDigit = (10 - (sum % 10)) % 10;

    // Check if the calculated check digit matches the 13th digit in the barcode
    return (checkDigit == barcodeStr[12] - '0');
}

// Function to validate UPC-A barcode
bool is_upca_barcode(long long barcode) {
    // Convert the long long barcode to a string for easier manipulation
    char barcodeStr[13];
    snprintf(barcodeStr, sizeof(barcodeStr), "%lld", barcode);

    // Check if the barcode is 12 digits long
    if (strlen(barcodeStr) != 12) {
        return false;
    }

    // Calculate the check digit
    int sum = 0;
    for (int i = 0; i < 12; i++) {
        int digit = barcodeStr[i] - '0';
        if (i % 2 == 0) {
            sum += 3 * digit;
        } else {
            sum += digit;
        }
    }

    int checkDigit = (10 - (sum % 10)) % 10;

    // Check if the calculated check digit matches the 12th digit in the barcode
    return (checkDigit == barcodeStr[11] - '0');
}