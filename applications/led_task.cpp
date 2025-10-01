#include "cmsis_os.h"
#include "io/led/led.hpp"

sp::LED led(&htim5);

extern "C" void led_task()
{
  led.start();

  while (true) {
    // Red从暗到亮
    for (uint8_t brightness = 0; brightness <= 10; brightness++) {
      led.set(brightness * 0.1f, 0.0f, 0.0f);
      osDelay(50);
    }

    // Red从亮到暗
    for (uint8_t brightness = 10; brightness > 0; brightness--) {
      led.set(brightness * 0.1f, 0.0f, 0.0f);
      osDelay(50);
    }

    // Green从暗到亮
    for (uint8_t brightness = 0; brightness <= 10; brightness++) {
      led.set(0.0f, brightness * 0.1f, 0.0f);
      osDelay(50);
    }

    // Green从亮到暗
    for (uint8_t brightness = 10; brightness > 0; brightness--) {
      led.set(0.0f, brightness * 0.1f, 0.0f);
      osDelay(50);
    }

    // Blue从暗到亮
    for (uint8_t brightness = 0; brightness <= 10; brightness++) {
      led.set(0.0f, 0.0f, brightness * 0.1f);
      osDelay(50);
    }

    // Blue从亮到暗
    for (uint8_t brightness = 10; brightness > 0; brightness--) {
      led.set(0.0f, 0.0f, brightness * 0.1f);
      osDelay(50);
    }
  }
}