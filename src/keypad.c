#include "stm32f0xx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "keypad.h"

uint8_t hist[16];
char queue[2];  // A two-entry queue of button press/release events.
int qin;        // Which queue entry is next for input
int qout;       // Which queue entry is next for output

// const char keymap[] = "DCBA#9630852*741";


char disp[9]         = "Hello...";
uint8_t mode         = 'A';
uint8_t thrust       = 0;
extern char *keymap;
extern char disp[9];
extern uint8_t mode;
char* keymap_arr = &keymap;

void enable_ports() {
    RCC -> AHBENR |= RCC_AHBENR_GPIOCEN; // Enables the RCC clock to GPIOB and GPIOC

    GPIOC -> MODER &= ~0x0000FF00; // PC4 – PC7 to be outputs
    GPIOC -> MODER |= 0x00005500;

    GPIOC -> PUPDR &= ~0x000000FF; // PC0 – PC3 to be inputs
    GPIOC -> PUPDR |= 0x000000AA; // PC0 – PC3 to be internally pulled low
}

void drive_column(int c) {
    c = c & 3;
    GPIOC -> BRR |= 0xF << 4; // turn off
    GPIOC -> BSRR |= 1 << (c + 4); // turn on
}

int read_rows() {
    return (GPIOC -> IDR) & 0xF;
}

char rows_to_key(int rows) {
    col = col & 3;
    int row = 0;
    if (rows & 0b1) {
      row = 0;
    }
    else if (rows & 0b10) {
      row = 1;
    }
    else if (rows & 0b100) {
      row = 2;
    }
    else if (rows & 0b1000) {
      row = 3;
    }
    int offset = col * 4 + row;
    char c = keymap_arr[offset];
    return c;
}
