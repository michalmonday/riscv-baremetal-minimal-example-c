

int function_a(char a, char b, char c) {
}

int function_b(char a[30], short *b, int c) {
}

int function_c(char a, char b, char c, char d, char e, char f, char g, char h, char i, char j, char k) {
}

int function_d(unsigned int a, unsigned int b, unsigned int c) {
}

int main() {
    function_a(1,2,3);
    char a[30];
    short b;
    function_b(a, &b, 3);
    function_c(1,2,3,4,5,6,7,8,9,10,11);
    unsigned int c = 0xABABABAB;
    function_d(c, 0x80000000, 0);

    return 0;
}
