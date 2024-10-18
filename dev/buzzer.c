#include "pico/stdlib.h"
#include "hardware/pwm.h"

static inline void pwm_calcDivTop(pwm_config *c, int frequency, int sysClock)
{
    uint  count = sysClock * 16 / frequency;
    uint  div =  count / 60000;

    if(div < 16) div = 16;

    c->div = div;
    c->top = count / div;
}

void beep_sound_play(int times)
{
    gpio_set_function(26, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(26);

    pwm_config cfg = pwm_get_default_config();
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
    int  wholenote = (60000 * 4) / 140;
    int loop;
    int duration;

    duration = 4;

    if(duration == 0)
        return;

    if(duration > 0)
        duration = wholenote / duration;
    else
        duration = (3 * wholenote / (-duration)) / 2;

    pwm_set_enabled(slice_num, false);
    pwm_calcDivTop(&cfg, 784, 125000000);
    pwm_init(slice_num, &cfg, true);

    for(int i=0 ; i<times ; i++) {
        pwm_set_chan_level(slice_num, PWM_CHAN_A, cfg.top / 2);
        pwm_set_enabled(slice_num, true);
        sleep_us(900 * duration);
        pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
        sleep_us(100 * duration);
    }
}