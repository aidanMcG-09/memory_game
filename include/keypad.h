#ifndef KEYPAD_H
#define KEYPAD_H

extern uint8_t col;



void enable_ports();
void drive_column(int c);
int read_rows();
char rows_to_key(int rows);
void setup_tim7();

char get_keypress();

void show_keys(void);

#endif