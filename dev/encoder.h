#ifndef __ENCODER_H__
#define __ENCODER_H__

#define ENC_DEBOUNCE_TIME (50)

void encoder_init();
int32_t get_encoder_value();

#endif // __ENCODER_H__