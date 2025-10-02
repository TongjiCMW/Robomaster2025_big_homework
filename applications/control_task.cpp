#include "cmsis_os.h"
#include "io/can/can.hpp"
#include "io/dbus/dbus.hpp"
#include "motor.hpp"
#include "motor/rm_motor/rm_motor.hpp"
#include "tools/pid/pid.hpp"
extern sp::DBus remote_controller;

extern sp::CAN can2;

extern sp::RM_Motor motor3508_1;

//初始化电机pid控制器以及电机运动数据
//                             dt     kp    ki    kd    mo   mio   alpha  ang? dynamic?
sp::PID motor3508_1_pid_speed(0.01f, 0.8f, 0.0f, 0.0f, 1.5f, 0.0f, 1.0f, false, true);
MovingData motor3508_1_data;

extern "C" void control_task()
{
  remote_controller.request();  //这里开始的时候要初始化,等待接收第一帧

  //can初始化配置
  can2.config();
  can2.start();
  //遥控器右边拨杆上中下挡控制输入给电机的电压值分别为5.5V, 2.0V, 0V
  //现在我上面的motor_type选择的是GM3508_V,所以cmd函数的输入单位是V

  //电机运动数据初始化

  motor3508_1_data.absolute_speed_set = 12.56f;
  motor3508_1_data.given_torque = 0.0f;
  motor3508_1_data.given_voltage = 0.0f;

  while (true) {
    // 使用调试(f5)查看remote_controller内部变量的变化
    //这里执行遥控器控制任务
    switch (remote_controller.sw_r) {
      case sp::DBusSwitchMode::UP:
        motor3508_1_data.given_torque = 0.2f;
        motor3508_1.cmd(motor3508_1_data.given_torque);
        break;
      case sp::DBusSwitchMode::MID:
        motor3508_1_pid_speed.calc(motor3508_1_data.absolute_speed_set, motor3508_1.speed);
        motor3508_1_data.given_torque = motor3508_1_pid_speed.out;
        motor3508_1.cmd(motor3508_1_data.given_torque);
        break;

        //约定右down挡时全部电机失能
        //这里一定是失能而不应该是急刹车
      case sp::DBusSwitchMode::DOWN:
        motor3508_1_data.given_torque = 0.0f;
        motor3508_1.cmd(motor3508_1_data.given_torque);
        break;
      default:
        break;
    }

    motor3508_1.write(can2.tx_data);
    can2.send(motor3508_1.tx_id);
    osDelay(10);
  }
}
