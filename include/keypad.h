#ifndef KEYPAD_H
#define KEYPAD_H

void push_queue(int n);

char pop_queue();

void update_history(int c, int rows);
void drive_column(int c);
int read_rows();
char get_key_event(void);

char get_keypress();

void show_keys(void);

#endif