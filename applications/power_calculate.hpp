
// power_control.hpp
#ifndef POWER_CONTROL_HPP
#define POWER_CONTROL_HPP

struct PowerControl
{
  // 功率预测系数 - 需要自己整定
  float K1 = 2.8f;    // 扭矩平方项系数
  float K2 = 0.009f;  // 转速平方项系数
  float K3 = 3.90f;   // 常数项
  float K = 1.0f;     // 线性缩放扭矩系数

  // 功率控制参数
  float safety_margin = 4.0f;        // 安全余量
  float capacitor_on_power = 20.0f;  // 电容策略功率加成
  float correction_factor = 0.92f;   // 修正系数

  // 功率预测值
  float power_prediction = 0.0f;
  float realtime_power_max = 0.0f;  // 实时功率最大值
};

extern PowerControl chassis_power_control;

#endif