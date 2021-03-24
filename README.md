EFM32 Stepper Motor Library
===========================

This library can be used to drive stepper motors using an EFM32 microcontroller. Here is a small example of how to initialize a motor and step one revolution using half-step mode.

```C
static stepper_t motor;

void motor_init(void) {
  pin_t pin1 = {.port=gpioPortE, .pin=10};
  pin_t pin2 = {.port=gpioPortE, .pin=11};
  pin_t pin3 = {.port=gpioPortE, .pin=12};
  pin_t pin4 = {.port=gpioPortE, .pin=13};
  stepper_init(&motor, 200, STEP_HALF, pin1, pin2, pin3, pin4);
  stepper_set_speed(&motor, 20);
  stepper_step(&motor, 400);
}
```
