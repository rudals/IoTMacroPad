/*******************************************************************************
MIT License

Copyright (c) 2023 Oswaldo Hernandez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "keypad.h"

#define GPIO_INPUT false
#define GPIO_OUTPUT true

uint _columns[4] = { 2, 3, 4, 5 };
uint _rows[2] = { 1, 0 };
char _matrix_values[8] = { '1', '2', '3', '4', '5', '6', '7', '8' };
uint all_columns_mask = 0x0;
uint column_mask[4];

void pico_keypad_init(void) {
    for (int i = 0; i < 2; i++) {
        gpio_init(_rows[i]);
        gpio_set_dir(_rows[i], GPIO_OUTPUT);
        gpio_put(_rows[i], 1);
    }

    for (int i = 0; i < 4; i++) {
        gpio_init(_columns[i]);
        gpio_set_dir(_columns[i], GPIO_INPUT);
        all_columns_mask = all_columns_mask + (1 << _columns[i]);
        column_mask[i] = 1 << _columns[i];
    }
}

char pico_keypad_get_key(void) {
    int row;
    uint32_t cols;
    bool pressed = false;

    cols = gpio_get_all();
    cols = cols & all_columns_mask;

    if (cols == 0x0) {
        return 0;
    }
    
    for (int j = 0; j < 2; j++) {
        gpio_put(_rows[j], 0);
    }

    for (row = 0; row < 2; row++) {
        gpio_put(_rows[row], 1);

        busy_wait_us(10000);

        cols = gpio_get_all();
        gpio_put(_rows[row], 0);
        cols = cols & all_columns_mask;
        if (cols != 0x0) {
            break;
        }
    }

    for (int i = 0; i < 2; i++) {
        gpio_put(_rows[i], 1);
    }
    
    if (cols == column_mask[0]) {
        return (char)_matrix_values[row * 4 + 0];
    }
    else if (cols == column_mask[1]) {
        return (char)_matrix_values[row * 4 + 1];
    }
    if (cols == column_mask[2]) {
        return (char)_matrix_values[row * 4 + 2];
    }
    else if (cols == column_mask[3]) {
        return (char)_matrix_values[row * 4 + 3];
    }
    else {
        return 0;
    }
}