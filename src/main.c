/**
  ******************************************************************************
  * @file    main.c
  * @author  Weili An, Niraj Menon
  * @date    Feb 7, 2024
  * @brief   ECE 362 Lab 7 student template
  ******************************************************************************
*/

/*******************************************************************************/

// Fill out your username!  Even though we're not using an autotest, 
// it should be a habit to fill out your username in this field now.
const char* username = "amcgooga";

/*******************************************************************************/ 

#include "stm32f0xx.h"
#include <stdint.h>
#include "commands.h"
#include "lcd.h"
#include "keypad.h"
#include "score.h"

void nano_wait(unsigned int n);

void micro_wait(unsigned int n) {
    for (int i = 0; i < n; i++) {
        nano_wait(1000);
    }
}

#define STEP4

void internal_clock();

// Uncomment only one of the following to test each step
// #define STEP1
// #define STEP2
// #define STEP3
// #define STEP4

void init_usart5() {
    // TODO
    RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC -> AHBENR |= RCC_AHBENR_GPIODEN;

    GPIOC -> MODER &= ~GPIO_MODER_MODER12;
    GPIOC -> MODER |= GPIO_MODER_MODER12_1;
    GPIOC -> AFR[1] |= 0x00020000;

    GPIOD -> MODER &= ~GPIO_MODER_MODER2;
    GPIOD -> MODER |= GPIO_MODER_MODER2_1;
    GPIOD -> AFR[0] |= 0x00000200;

    RCC -> APB1ENR |= RCC_APB1ENR_USART5EN;
    USART5 -> CR1 &= ~USART_CR1_UE;
    USART5 -> CR1 &= ~USART_CR1_M0;
    USART5 -> CR1 &= ~USART_CR1_M1;
    USART5 -> CR2 &= ~USART_CR2_STOP;
    USART5 -> CR1 &= ~USART_CR1_PCE;
    USART5 -> CR1 &= ~USART_CR1_OVER8;
    USART5 -> BRR = 0x1A1;
    USART5 -> CR1 |= USART_CR1_TE;
    USART5 -> CR1 |= USART_CR1_RE;
    USART5 -> CR1 |= USART_CR1_UE;
    while(!(USART5->ISR & USART_ISR_TEACK) && !(USART5->ISR & USART_ISR_REACK));
}

#ifdef STEP1
int main(void){
    internal_clock();
    init_usart5();
    for(;;) {
        while (!(USART5->ISR & USART_ISR_RXNE)) { }
        char c = USART5->RDR;
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = c;
    }
}
#endif

#ifdef STEP2
#include <stdio.h>

// TODO Resolve the echo and carriage-return problem

int __io_putchar(int c) {
    // TODO
    if (c == '\n') {
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
    
    return c;
}

int __io_getchar(void) {
    while (!(USART5->ISR & USART_ISR_RXNE));
    char c = USART5->RDR;
    // TODO
    if (c == '\r') {
        c = '\n';
    }
    __io_putchar(c);
    return c;
}

int main() {
    internal_clock();
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef STEP3
#include <stdio.h>
#include "fifo.h"
#include "tty.h"
int __io_putchar(int c) {
    // TODO Copy from your STEP2
    if (c == '\n') {
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
    
    return c;
}

int __io_getchar(void) {
    // TODO
    return line_buffer_getchar();
}

int main() {
    internal_clock();
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef STEP4

#include <stdio.h>
#include "fifo.h"
#include "tty.h"

// TODO DMA data structures
#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;

void enable_tty_interrupt(void) {
    // TODO
    NVIC -> ISER[0] |= 1 << USART3_8_IRQn;
    USART5 -> CR1 |= USART_CR1_RXNEIE;
    USART5 -> CR3 |= USART_CR3_DMAR;

    RCC->AHBENR |= RCC_AHBENR_DMA2EN;
    DMA2->CSELR |= DMA2_CSELR_CH2_USART5_RX;
    DMA2_Channel2->CCR &= ~DMA_CCR_EN;  // First make sure DMA is turned off
    
    // The DMA channel 2 configuration goes here
    DMA2_Channel2 -> CMAR = &serfifo;
    DMA2_Channel2 -> CPAR = &(USART5->RDR);
    DMA2_Channel2 -> CNDTR = FIFOSIZE;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_DIR;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_TCIE;// total completion?
    DMA2_Channel2 -> CCR &= ~DMA_CCR_HTIE;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_PSIZE;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_MSIZE;
    DMA2_Channel2 -> CCR |= DMA_CCR_MINC;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_PINC;
    DMA2_Channel2 -> CCR |= DMA_CCR_CIRC;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_MEM2MEM;
    DMA2_Channel2 -> CCR |= DMA_CCR_PL;
    DMA2_Channel2->CCR |= DMA_CCR_EN;
}

// Works like line_buffer_getchar(), but does not check or clear ORE nor wait on new characters in USART
char interrupt_getchar() {
    // TODO
    // USART_TypeDef *u = USART5;
    // // If we missed reading some characters, clear the overrun flag.
    // if (u->ISR & USART_ISR_ORE)
    //     u->ICR |= USART_ICR_ORECF;
    // Wait for a newline to complete the buffer.
    while(fifo_newline(&input_fifo) == 0) {
        // while (!(u->ISR & USART_ISR_RXNE))
        //     ;
        // insert_echo_char(u->RDR);
        asm volatile ("wfi"); // wait for an interrupt
    }
    // Return a character from the line buffer.
    char ch = fifo_remove(&input_fifo);
    return ch;
}

int __io_putchar(int c) {
    // TODO copy from STEP2
    if (c == '\n') {
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
    
    return c;
}

int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()
    return interrupt_getchar();
}

// TODO Copy the content for the USART5 ISR here
// TODO Remember to look up for the proper name of the ISR function
void USART3_8_IRQHandler(void) {
    while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo))
            insert_echo_char(serfifo[seroffset]);
        seroffset = (seroffset + 1) % sizeof serfifo;
    }
}

// tft funcs __________________________________________________________________________________________________
void init_spi1_slow(void) {
    RCC -> APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB -> MODER &= ~0x00000FC0;
    GPIOB -> MODER |= 0x000000A80;
    GPIOB -> AFR[0] &= ~0x00FFF000; //AF0 for 3,4,5
    SPI1 -> CR1 &= ~SPI_CR1_SPE;
    SPI1 -> CR1 |= SPI_CR1_BR; // high as possible?
    SPI1 -> CR1 |= SPI_CR1_MSTR; // this or cr2_ssoe ?
    SPI1 -> CR2 |= SPI_CR2_SSOE;
    SPI1 -> CR2 &= ~SPI_CR2_DS;
    SPI1 -> CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2); // 8-bit data size
    SPI1 -> CR1 |= SPI_CR1_SSM;
    SPI1 -> CR1 |= SPI_CR1_SSI;
    SPI1 -> CR2 |= SPI_CR2_FRXTH;
    SPI1 -> CR1 |= SPI_CR1_SPE;
}

void enable_sdcard()
{
    //set PB2 to low
    GPIOB -> ODR &= ~(1 << 2);
  
}
void disable_sdcard()
{
    //set PB2 high
    GPIOB -> ODR |= (1 << 2);

}
void init_sdcard_io()
{
    init_spi1_slow();
    //configure PB2 as output
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB -> MODER |= (GPIO_MODER_MODER2_0);
    GPIOB -> MODER &= ~(GPIO_MODER_MODER2_1);
    disable_sdcard();
}

void sdcard_io_high_speed()
{
    //set SP1 B
    SPI1 -> CR1 &= ~SPI_CR1_SPE;
    SPI1 -> CR1 &= ~SPI_CR1_BR;
    SPI1 -> CR1 |= SPI_CR1_BR_0;
    SPI1 -> CR1 |= SPI_CR1_SPE;

}

void init_lcd_spi(){
    //PB8, PB11, PB14 as GPIO outputs
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB -> MODER |= (GPIO_MODER_MODER8_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0);
    GPIOB -> MODER &= ~(GPIO_MODER_MODER8_1 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER14_1);
    //call init_spi_slow
    init_spi1_slow();
    //call sdcard_io_high_speed
    sdcard_io_high_speed();
}
// __________________________________________________________________________________________________________________________
// Game logic
// __________________________________________________________________________________________________________________________

char flash_string[] = "hello";  // Buffer to hold the string that will be flashed
char user_input[10];    // Buffer to store user input for comparison

// Function to display the flash string for a limited time
void flash_string_on_screen(int duration) {
    LCD_Clear(0xffff);
    LCD_DrawString(10, 10, 0x0000, 0xffff, flash_string, 16, 1);
    micro_wait(duration);  // Wait for the specified duration (in milliseconds)
    LCD_Clear(0xffff);  // Clear the screen after flashing
}

// // Function to get user input and compare with flashed string
// int get_and_check_user_input(int length) {
//     for (int i = 0; i < length; i++) {
//         user_input[i] = get_keypress();
//         // Display the current user input (optional visual feedback)
//         tft_draw_text(10, 30 + i * 10, user_input);
//     }
//     user_input[length] = '\0';  // Null-terminate the string

//     // Compare the user input with the flash string
//     if (strcmp(flash_string, user_input) == 0) {
//         return 1;  // Correct input
//     } else {
//         return 0;  // Incorrect input
//     }
// }


// // Display Game Instructions
// void display_instructions(void) {
//     tft_clear();
//     tft_draw_text(10, 10, "Game Instructions:");
//     tft_draw_text(10, 30, "Press A to Start");
//     tft_draw_text(10, 50, "Move Left: 4");
//     tft_draw_text(10, 70, "Move Right: 7");
//     tft_draw_text(10, 90, "Pause: *");
//     tft_draw_text(10, 110, "Quit: #");
//     nano_wait(5000);  // Wait for 5 seconds before starting the game
// }

// void game(void) {
//     tft_init();  // Initialize TFT
//     tft_clear();  // Clear the screen

//     // Start the game when 'A' is pressed
//     while (1) {
//         char key = get_keypress();
//         if (key == 'A') {
//             // Game start logic
//             score = 0;  // Reset score
//             int level = 4;  // Starting string length
//             while (1) {
//                 generate_flash_string(level); 
//                 flash_string_on_screen(flash_duration);  // Flash the string

//                 // Get user input and check if it's correct
//                 if (get_and_check_user_input(level)) {
// 		    if(level == 10){
// 			;
// 	            }
// 		    else{
// 		      level++;	
//                     }
                    
// 		    score++;
//                     snprintf(user_input, sizeof(user_input), "Score: %d", score);
//                     tft_draw_text(10, 100, user_input);  // Display current score
//                 } else {
//                     tft_draw_text(10, 100, "Game Over!");  // Game over
//                     break;
//                 }

//                 nano_wait(2000);  // Wait before next round
//             }
//         }
//     }
// }

// __________________________________________________________________________________________________________________________


int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();

    setbuf(stdin,0); // These turn off buffering; more efficient, but makes it hard to explain why first 1023 characters not dispalyed
    setbuf(stdout,0);
    setbuf(stderr,0);
    // printf("Enter your name: "); // Types name but shouldn't echo the characters; USE CTRL-J to finish
    // char name[80];
    // fgets(name, 80, stdin);
    // printf("Your name is %s", name);
    // printf("Type any characters.\n"); // After, will type TWO instead of ONE
    // for(;;) {
    //     char c = getchar();
    //     putchar(c);
    // }

    init_spi1_slow();
    enable_sdcard();
    disable_sdcard();
    init_sdcard_io();
    sdcard_io_high_speed();
    init_lcd_spi();

    //command_shell();

    LCD_Setup();
    mount_sd();
    save_score(65);

    flash_string_on_screen(5000000);

    printf("SCORE: %d\n", get_score());

}
#endif