
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <eembc_adaptation.h> // provides parse_args_from_stdin_csv, th_exit

// uart_gpio can communicate with another microcontroller through function like: 
// uart_gpio_puts, uart_gpio_can_send, uart_gpio_data_available, uart_gpio_getchar, uart_gpio_putchar, uart_gpio_printf, uart_gpio_scanf
// in the past it was used for communicating with Esp32 display for ECG project.
// Now it can be used to communicate with Esp32 display that acts as a login screen.
#include <uart_gpio.h>     
#include <uart_pynq.h>
#include <sensors.h>       // analog_inputs, digital_inputs
#include <utils_flute.h>   // wait_ms, wait_s, CLK_SPEED, get_ticks_count, get_overlay_ticks_count, get_random_number
#include <stdbool.h>

#define CORRECT_PASSWORD "1234"

bool is_barcode_known(char *barcode_str);
bool is_barcode_valid(unsigned long long barcode);
bool is_ean13_barcode(unsigned long long barcode);
bool is_upca_barcode(unsigned long long barcode);
unsigned long long barcode_str_to_num(char *barcode_str);

void main(void) {
    // cmd receives keyboard input and barcodes using the same uart_gpio interface 
    // (from the Esp32-based display board), it is used for:
    // - receiving password
    // - receiving barcodes
    // - logout from barcode scanning (access control) part of the program (back into login screen)
    char cmd[64] = {0};
    char *barcode_str = cmd;
    char login_buffer[32] = {0};

    // vulnerable password buffer
    char password_buffer[4] = {0};
    
    // variable responsible for admin login authorization
    int is_authorized = 0;

LOGIN:
    while(!is_authorized) {
        while (!uart_gpio_data_available()) {
            wait_ms(500);
        }
        // let all characters arrive
        wait_ms(300);
        // gets/scanf are vulnerable to buffer overflow, if we supply more than 4 characters, 
        // we can overwrite the is_authorized variable without knowing the correct password
        uart_gpio_gets(password_buffer);
        if(!strcmp(password_buffer, CORRECT_PASSWORD)) {
            is_authorized = 1;
        } else {
            uart_gpio_puts("Incorrect password!");
        }
    }
    uart_gpio_puts("Access granted!");

    while (true) {
        while (!uart_gpio_data_available()) {
            wait_ms(500);
        }
        // let all characters arrive
        wait_ms(300);
        uart_gpio_gets(cmd);
        if (!strcmp(cmd, "logout")) {
            uart_gpio_puts("Logged out");
            is_authorized = 0;
            goto LOGIN;
        }
        unsigned long long barcode = barcode_str_to_num(barcode_str);
        // check validity of barcode just for the sake of processing it in some way
        bool is_valid = is_barcode_valid(barcode);
        if (is_barcode_known(barcode_str)) {
            uart_gpio_puts("Known ID: doors opened");
        } else {
            uart_gpio_puts("Unknown ID: doors closed");
        }
    }
}


bool is_barcode_known(char *barcode) {
    // Sending "bc:12908409184\n" to pynq will make it lookup sqlite database 
    // (that is automatically initialized with default values on boot)
    // This is vulnerable because the database check it made in the 
    // following way:
    //
    //    def is_barcode_in_db(self, barcode):
    //        # secure:
    //        # self.c.execute("SELECT * FROM allowed_barcode_IDs WHERE barcode_ID = ?", (barcode,))
    // 
    //        # vulnerable to sql injection:
    //        self.c.execute(f"SELECT * FROM allowed_barcode_IDs WHERE barcode_ID = {barcode}")
    // 
    //        return self.c.fetchone() is not None  
    // 
    // We can send any value followed by "OR 1=1" (e.g. "bc:123 OR 1=1\n") and it will return true (1)
    // regardless if the barcode is in the database or not.
    printf("bc:%s\n", barcode);
    int is_in_db = false;
    char response[64] = {0};
    wait_ms(500);
    // get response from SQL database on PYNQ
    gets(response);
    sscanf(response, "bc:%d", &is_in_db);
    return is_in_db > 0;
}
                                                    
bool is_barcode_valid(unsigned long long barcode) {
    return is_ean13_barcode(barcode) || is_upca_barcode(barcode);
}

bool is_ean13_barcode(unsigned long long barcode) {
    char barcodeStr[14];
    snprintf(barcodeStr, sizeof(barcodeStr), "%lld", barcode);
    if (strlen(barcodeStr) != 13) return false;
    int sum = 0;
    for (int i = 0; i < 12; i++) {
        int digit = barcodeStr[i] - '0';
        if (i % 2 == 0)
            sum += digit;
        else
            sum += 3 * digit;
    }
    int checkDigit = (10 - (sum % 10)) % 10;
    return (checkDigit == barcodeStr[12] - '0');
}

bool is_upca_barcode(unsigned long long barcode) {
    char barcodeStr[13];
    snprintf(barcodeStr, sizeof(barcodeStr), "%lld", barcode);
    if (strlen(barcodeStr) != 12) return false;
    int sum = 0;
    for (int i = 0; i < 12; i++) {
        int digit = barcodeStr[i] - '0';
        if (i % 2 == 0) 
            sum += 3 * digit;
        else
            sum += digit;
    }
    int checkDigit = (10 - (sum % 10)) % 10;
    return (checkDigit == barcodeStr[11] - '0');
}

unsigned long long barcode_str_to_num(char *barcode_str) {
    unsigned long long barcode = 0;
    sscanf(barcode_str, "%llu", &barcode);
    return barcode;
}