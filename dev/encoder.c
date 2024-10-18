#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hardware/pio.h"
#include "quadrature.pio.h"

#define QUADRATURE_A_PIN 7
#define QUADRATURE_B_PIN 6
#define QUADRATURE_SW_PIN 8

PIO pio = pio0;
uint offset, sm;

int32_t get_encoder_value()
{
    pio_sm_exec_wait_blocking(pio, sm, pio_encode_in(pio_x, 32));
    int32_t encoder_value = pio_sm_get_blocking(pio, sm);
    encoder_value >>= 1;
    return encoder_value;
}

void quadrature_sw_callback(uint gpio, uint32_t events) {
    if(gpio == QUADRATURE_SW_PIN) {
        printf("Pressed\n");
    }
}

void encoder_init()
{
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &quadratureB_program);
    uint sm = pio_claim_unused_sm(pio, true);
    quadratureB_program_init(pio, sm, offset, QUADRATURE_A_PIN, QUADRATURE_B_PIN);
    gpio_set_irq_enabled_with_callback(QUADRATURE_SW_PIN, GPIO_IRQ_EDGE_FALL, true, &quadrature_sw_callback);
    pio_sm_exec(pio, sm, pio_encode_set(pio_x, 0));
}