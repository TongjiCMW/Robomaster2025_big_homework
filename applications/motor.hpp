#ifndef MOTOR_HPP
#define MOTOR_HPP
#include "motor/rm_motor/rm_motor.hpp"

typedef struct
{
  float absolute_angle_set;  // 绝对角度的目标值，rad
  float absolute_speed_set;  // 绝对速度的目标值，rad/s
  float given_torque;        // 电机给定的力矩，Nm
  float given_voltage;       // 电机给定电压，V
} MovingData;

#endif  // MOTOR_HPP