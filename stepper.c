#include "stepper.h"
#include "em_gpio.h"
#include "math.h"
#include "sl_sleeptimer.h"

static void step_update_full(stepper_t *m, int step_number);
static void step_update_half(stepper_t *m, int step_number);

void stepper_init(stepper_t *m, short num_steps, step_mode_t mode, pin_t pin1, pin_t pin2, pin_t pin3, pin_t pin4)
{
  // Initial state of motor
  GPIO_PinModeSet(pin1.port, pin1.pin, gpioModePushPull, 1); // IN1 (A-)
  GPIO_PinModeSet(pin2.port, pin2.pin, gpioModePushPull, 0); // IN2 (A+)
  GPIO_PinModeSet(pin3.port, pin3.pin, gpioModePushPull, 1); // IN3 (B-)
  GPIO_PinModeSet(pin4.port, pin4.pin, gpioModePushPull, 0); // IN4 (B+)

  m->pin1 = pin1;
  m->pin2 = pin2;
  m->pin3 = pin3;
  m->pin4 = pin4;
  m->num_steps = num_steps;
  m->state = 0;
  m->mode = mode;
  if (mode == STEP_FULL) {
    m->step_update = step_update_full;
  } else {
    m->step_update = step_update_half;
  }
  // default speed
  stepper_set_speed(m, 10);
}

void stepper_set_speed(stepper_t *m, int rpm)
{
  // calculate delay in ms rounding up
  int steps = m->num_steps;
  if (m->mode == STEP_HALF) {
    steps = m->num_steps * 2; // double amount of steps per revolution when doing half-step
  }
  float delay_f = 60.0 * 1000.0 / (steps * rpm);
  m->delay = ceil(delay_f);
}

void stepper_step(stepper_t *m, int steps)
{
  int step_current = m->state;
  int step_target = step_current + steps;

  while (step_current != step_target) {
    // Move to next step in state machine
    if (step_target > step_current) {
      step_current++;
    } else {
      step_current--;
    }

    // Start with a delay to match the configured speed
    sl_sleeptimer_delay_millisecond(m->delay);
    m->step_update(m, step_current);
  }
  m->state = step_current;
}

static void step_update_full(stepper_t *m, int step_number)
{
  int state = step_number % 4;
  if (state < 0) {
    state = -state;
  }

  switch (state) {
    case 0: // 1010
      GPIO_PinOutSet(m->pin1.port, m->pin1.pin);
      GPIO_PinOutClear(m->pin2.port, m->pin2.pin);
      GPIO_PinOutSet(m->pin3.port, m->pin3.pin);
      GPIO_PinOutClear(m->pin4.port, m->pin4.pin);
      break;
    case 1: // 0110
      GPIO_PinOutClear(m->pin1.port, m->pin1.pin);
      GPIO_PinOutSet(m->pin2.port, m->pin2.pin);
      GPIO_PinOutSet(m->pin3.port, m->pin3.pin);
      GPIO_PinOutClear(m->pin4.port, m->pin4.pin);
      break;
    case 2: // 0101
      GPIO_PinOutClear(m->pin1.port, m->pin1.pin);
      GPIO_PinOutSet(m->pin2.port, m->pin2.pin);
      GPIO_PinOutClear(m->pin3.port, m->pin3.pin);
      GPIO_PinOutSet(m->pin4.port, m->pin4.pin);
      break;
    case 3: // 1001
      GPIO_PinOutSet(m->pin1.port, m->pin1.pin);
      GPIO_PinOutClear(m->pin2.port, m->pin2.pin);
      GPIO_PinOutClear(m->pin3.port, m->pin3.pin);
      GPIO_PinOutSet(m->pin4.port, m->pin4.pin);
      break;
  }
}

static void step_update_half(stepper_t *m, int step_number)
{
  int state = step_number % 8;
  if (state < 0) {
    state = -state;
  }

  switch (state) {
    case 0: // 1010
      GPIO_PinOutSet(m->pin1.port, m->pin1.pin);
      GPIO_PinOutClear(m->pin2.port, m->pin2.pin);
      GPIO_PinOutSet(m->pin3.port, m->pin3.pin);
      GPIO_PinOutClear(m->pin4.port, m->pin4.pin);
      break;
    case 1: // 0010
      GPIO_PinOutClear(m->pin1.port, m->pin1.pin);
      GPIO_PinOutClear(m->pin2.port, m->pin2.pin);
      GPIO_PinOutSet(m->pin3.port, m->pin3.pin);
      GPIO_PinOutClear(m->pin4.port, m->pin4.pin);
      break;
    case 2: // 0110
      GPIO_PinOutClear(m->pin1.port, m->pin1.pin);
      GPIO_PinOutSet(m->pin2.port, m->pin2.pin);
      GPIO_PinOutSet(m->pin3.port, m->pin3.pin);
      GPIO_PinOutClear(m->pin4.port, m->pin4.pin);
      break;
    case 3: // 0100
      GPIO_PinOutClear(m->pin1.port, m->pin1.pin);
      GPIO_PinOutSet(m->pin2.port, m->pin2.pin);
      GPIO_PinOutClear(m->pin3.port, m->pin3.pin);
      GPIO_PinOutClear(m->pin4.port, m->pin4.pin);
      break;
    case 4: // 0101
      GPIO_PinOutClear(m->pin1.port, m->pin1.pin);
      GPIO_PinOutSet(m->pin2.port, m->pin2.pin);
      GPIO_PinOutClear(m->pin3.port, m->pin3.pin);
      GPIO_PinOutSet(m->pin4.port, m->pin4.pin);
      break;
    case 5: // 0001
      GPIO_PinOutClear(m->pin1.port, m->pin1.pin);
      GPIO_PinOutClear(m->pin2.port, m->pin2.pin);
      GPIO_PinOutClear(m->pin3.port, m->pin3.pin);
      GPIO_PinOutSet(m->pin4.port, m->pin4.pin);
      break;
    case 6: // 1001
      GPIO_PinOutSet(m->pin1.port, m->pin1.pin);
      GPIO_PinOutClear(m->pin2.port, m->pin2.pin);
      GPIO_PinOutClear(m->pin3.port, m->pin3.pin);
      GPIO_PinOutSet(m->pin4.port, m->pin4.pin);
      break;
    case 7: // 1000
      GPIO_PinOutSet(m->pin1.port, m->pin1.pin);
      GPIO_PinOutClear(m->pin2.port, m->pin2.pin);
      GPIO_PinOutClear(m->pin3.port, m->pin3.pin);
      GPIO_PinOutClear(m->pin4.port, m->pin4.pin);
      break;
  }
}
