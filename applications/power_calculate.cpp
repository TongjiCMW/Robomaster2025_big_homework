#include "cmsis_os.h"
#include "motor/rm_motor/rm_motor.hpp"
#include "motor/super_cap/super_cap.hpp"

extern sp::SuperCap supercap;
extern sp::RM_Motor motor3508_1;
extern sp::RM_Motor motor3508_2;
extern sp::RM_Motor motor3508_3;
extern sp::RM_Motor motor3508_4;

// 功率预测系数 - 需要自己整定
float K1 = 2.0f;    // 扭矩平方项系数
float K2 = 0.1f;  // 转速平方项系数
float K3 = 5.80f;    // 常数项

// 需要从control_task传入的目标扭矩值
extern float motor3508_1_cmd_torque;
extern float motor3508_2_cmd_torque;
extern float motor3508_3_cmd_torque;
extern float motor3508_4_cmd_torque;

// 全局变量 - 供外部访问的功率预测值
float power_prediction = 0.0f;

// 滑动滤波相关变量
static float speed_buffer[4][5] = {{0}};  // 4个电机，每个电机5个历史数据
static int buffer_index = 0;
static bool buffer_full = false;

// 滑动滤波函数
float moving_average_filter(float new_value, int motor_index)
{
  speed_buffer[motor_index][buffer_index] = new_value;

  if (buffer_full) {
    return (speed_buffer[motor_index][0] + speed_buffer[motor_index][1] +
            speed_buffer[motor_index][2]) /
           3.0f;
  }
  else {
    // 缓冲区未满时使用当前值
    return new_value;
  }
}

// 功率预测函数
float predict_power()
{
  // 获取所有电机的目标扭矩和实际转速
  float target_torque_1 = motor3508_1_cmd_torque;  // 目标扭矩
  float target_torque_2 = motor3508_2_cmd_torque;
  float target_torque_3 = motor3508_3_cmd_torque;
  float target_torque_4 = motor3508_4_cmd_torque;

  // 对转速进行滑动滤波
  float speed_1 = moving_average_filter(motor3508_1.speed, 0);  // 滤波后的转速
  float speed_2 = moving_average_filter(motor3508_2.speed, 1);
  float speed_3 = moving_average_filter(motor3508_3.speed, 2);
  float speed_4 = moving_average_filter(motor3508_4.speed, 3);

  // 计算各项之和
  float sum_torque_speed = target_torque_1 * speed_1 + target_torque_2 * speed_2 +
                           target_torque_3 * speed_3 + target_torque_4 * speed_4;

  float sum_torque_square = target_torque_1 * target_torque_1 + target_torque_2 * target_torque_2 +
                            target_torque_3 * target_torque_3 + target_torque_4 * target_torque_4;

  float sum_speed_square =
    speed_1 * speed_1 + speed_2 * speed_2 + speed_3 * speed_3 + speed_4 * speed_4;

  // 计算预测功率
  float predicted_power = K3 + sum_torque_speed + K1 * sum_torque_square + K2 * sum_speed_square;

  return predicted_power;
}

extern "C" void power_calculate_task()
{
  while (true) {
    // 计算预测功率并更新全局变量
    power_prediction = predict_power();

    // 更新滑动滤波的缓冲区索引
    buffer_index = (buffer_index + 1) % 3;
    if (buffer_index == 0) buffer_full = true;

    // 可以在这里添加功率限制逻辑
    // 比如与 supercap.power_out 比较，进行功率管理

    osDelay(10);  // 100Hz
  }
}