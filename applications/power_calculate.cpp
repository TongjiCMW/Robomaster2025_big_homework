#include "power_calculate.hpp"

#include <cmath>

#include "cmsis_os.h"
#include "io/dbus/dbus.hpp"
#include "motor/rm_motor/rm_motor.hpp"
#include "motor/super_cap/super_cap.hpp"
#include "referee/pm02/pm02.hpp"
extern sp::SuperCap supercap;
extern sp::PM02 pm02;
extern sp::DBus remote_controller;
extern sp::RM_Motor motor3508_1;
extern sp::RM_Motor motor3508_2;
extern sp::RM_Motor motor3508_3;
extern sp::RM_Motor motor3508_4;

// 需要从control_task传入的目标扭矩值  全局变量
extern float motor3508_1_cmd_torque;
extern float motor3508_2_cmd_torque;
extern float motor3508_3_cmd_torque;
extern float motor3508_4_cmd_torque;

// 实例化功率控制结构体 - 供外部访问
PowerControl chassis_power_control;

// 滑动滤波相关变量,
static float speed_buffer[4][3] = {{0}};  // 4个电机，每个电机3个历史数据
static int buffer_index = 0;
static bool buffer_full = false;

// 滑动滤波函数  这个函数用来消除电机转速反馈值的噪声
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
  float predicted_power = chassis_power_control.K3 + sum_torque_speed +
                          chassis_power_control.K1 * sum_torque_square +
                          chassis_power_control.K2 * sum_speed_square;
  float power_max = pm02.robot_status.chassis_power_limit;
  //从裁判系统得到的底盘功率限制
  if (remote_controller.sw_r == sp::DBusSwitchMode::UP) {
    power_max += chassis_power_control.capacitor_on_power;
    // 电容模式下允许更高的功率
  }
  chassis_power_control.realtime_power_max = power_max;
  if (predicted_power > power_max - chassis_power_control.safety_margin) {
    chassis_power_control.K =
      chassis_power_control.correction_factor *
      (-sum_torque_speed +
       sqrt(
         (sum_torque_speed * sum_torque_speed) -
         4 * chassis_power_control.K1 * sum_torque_square *
           (chassis_power_control.K2 * sum_speed_square + chassis_power_control.K3 - power_max))) /
      (2 * chassis_power_control.K1 * sum_torque_square);
  }
  else {
    chassis_power_control.K = 1.0f;  // 不需要缩放
  }
  return predicted_power;
}

extern "C" void power_calculate_task()
{
  while (true) {
    // 计算预测功率并更新全局变量
    chassis_power_control.power_prediction = predict_power();

    // 更新滑动滤波的缓冲区索引
    buffer_index = (buffer_index + 1) % 3;
    if (buffer_index == 0) buffer_full = true;

    // 可以在这里添加功率限制逻辑
    // 比如与 supercap.power_out 比较，进行功率管理

    osDelay(10);  // 100Hz
  }
}