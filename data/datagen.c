#include <ctype.h>
#define NO_LOWERCASE
#include "font.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

int get_font_index(int code) {
    int i =0;
    while (font[i].letter != 0) {
        if (font[i].letter == code) {
            return i;
        }
        i += 1;
    }
    return -1;
}

struct Font empty = {
    ' ', { /* Processor should ignore this */
        "     ",
        "     ",
        "     ",
        "     ",
        "     ",
        "     ",
        "     "
    }
};


int main() {

    // int i = 0;
    printf("module rom (input wire [6:0] code, input wire [2:0] y, output wire [5:0] row);\n");
    printf("\nreg [5:0] font[127:0][7:0];\n");
    printf("initial begin\n");
    for (int c = 0; c < 128; ++c) {
        int idx = get_font_index(toupper(c));
        struct Font * f;
        if (idx >= 0) {
            f = &font[idx];
        } else {
            f = &empty;
        }
        for (int y = 0; y < 8; ++y) {
            printf("font[%3d][%d] = 6'b", c, y);
            for (int x = 5; x >= 0 ; --x) {
                if (y == 7 || x > 4) {
                    putchar('0');
                }
                else if (f->code[y][x] == '#') {
                    putchar('1');
                } else {
                    putchar('0');
                }
            }
            printf(";\n");
        }
    }


    printf("end\n");
    printf("\nassign row = font[code][y];\n");

    printf("endmodule\n");
}
