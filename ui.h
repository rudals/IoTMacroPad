#pragma once

extern uint8_t _CurrentFontheight;

void disp_init();
void disp_menu();
void disp_key(int index);
void disp_log(const char* str);
void disp_cmd(int index);
void get_cmd_data(uint8_t *buf, int index);
void set_cmd_data(int main, int sub, uint8_t *tag, uint8_t *cmd);