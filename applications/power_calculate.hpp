
// power_control.hpp
#ifndef POWER_CONTROL_HPP
#define POWER_CONTROL_HPP

#include <math.h>
struct PowerControl
{
  // 功率预测系数 - 需要自己整定
  float K1 = 2.0f;    // 扭矩平方项系数
  float K2 = 0.005f;  // 转速平方项系数
  float K3 = 3.90f;   // 常数项
  float K = 1.0f;     // 线性缩放扭矩系数

  // 功率控制参数
  float safety_margin = 10.0f;        // 安全余量
  float capacitor_on_power = 20.0f;  // 电容策略功率加成
  float correction_factor = 0.92f;
  // 修正系数 这个参数是用来修正K的,如果在安全余量下还是会超功率,就把这个值调小一些

  // 功率预测值
  float power_prediction = 0.0f;
  float realtime_power_max = 0.0f;    // 实时功率最大值
  bool power_control_on_flag = true;  // 是否开启功率控制

  // 微分器相关变量
  float last_predicted_power = 0.0f;                // 上一次的预测功率
  float power_derivative = 0.0f;                    // 功率变化率
  float derivative_threshold = 700.0f;              // 功率急剧上升阈值 (W/s)
  float correction_factor_decrease_rate = 0.05f;    // 修正系数减少速率
  float correction_factor_min = 0.7f;               // 修正系数最小值
  float correction_factor_recovery_rate = 0.0025f;  // 修正系数恢复速率
};

extern PowerControl chassis_power_control;

#endif