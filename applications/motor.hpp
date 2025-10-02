#ifndef MOTOR_HPP
#define MOTOR_HPP

//#include "motor/lk_motor/lk_motor.hpp"
#include "motor/rm_motor/rm_motor.hpp"
//#include "motor/dm_motor/dm_motor.hpp"

/*
constexpr float P_MIN = -3.141593f * 4;
constexpr float P_MAX = 3.141593f * 4;
constexpr float V_MIN = -30.0f;
constexpr float V_MAX = 30.0f;
constexpr float T_MIN = -10.0f;
constexpr float T_MAX = 10.0f;

inline sp::LK_Motor lk_motor_x(2, 0.06);
inline sp::RM_Motor rm_motor_x(3, sp::RM_Motors::M3508);
inline sp::DM_Motor dm_motor_x(0x17, 0x18, P_MAX, V_MAX, T_MAX);


// 电机使能
inline bool motor_enable = false;       // 电机使能标志
inline uint16_t motor_enable_freq = 0;  // 电机使能计数器
*/

typedef struct
{
  float absolute_angle_set;  // 绝对角度的目标值，rad
  float absolute_speed_set;  // 绝对速度的目标值，rad/s
  float given_torque;        // 电机给定的力矩，Nm
  float given_voltage;       // 电机给定电压，V
} MovingData;

#endif  // MOTOR_HPP