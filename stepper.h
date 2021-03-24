#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>

typedef struct pin_t {
  uint8_t port;
  uint8_t pin;
} pin_t;

typedef enum step_mode {
  STEP_FULL,
  STEP_HALF,
} step_mode_t;

struct stepper;

typedef struct stepper {
  short num_steps; // full steps per revolution
  pin_t pin1;
  pin_t pin2;
  pin_t pin3;
  pin_t pin4;
  int delay;
  int state;
  step_mode_t mode;
  void (*step_update)(struct stepper *m, int step_number);
} stepper_t;

void stepper_init(stepper_t *m, short num_steps, step_mode_t mode, pin_t pin1, pin_t pin2, pin_t pin3, pin_t pin4);
void stepper_set_speed(stepper_t *m, int rpm);
void stepper_set_mode(stepper_t *m, step_mode_t mode);
void stepper_step(stepper_t *m, int steps);

#endif
