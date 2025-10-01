#include "cmsis_os.h"
#include "io/servo/servo.hpp"

// C板
sp::Servo servo(&htim1, TIM_CHANNEL_1, 168e6f, 180.0f);  // 开发板最上面的PWM端口, 180度舵机

// 达妙
// sp::Servo servo(&htim1, TIM_CHANNEL_3, 240e6f, 180.0f); // 开发板最上面的PWM端口, 180度舵机

extern "C" void servo_task()
{
  servo.start();

  for (float angle = 0; angle <= 181; angle += 45) {
    servo.set(angle);
    osDelay(1000);
  }
  vTaskDelete(NULL);//这里完成一遍后把自己删掉,不然就轧钢了卡死了
}