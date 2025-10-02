
#include "cmsis_os.h"
#include "io/can/can.hpp"
#include "io/dbus/dbus.hpp"
#include "motor.hpp"
#include "motor/rm_motor/rm_motor.hpp"
#include "tools/pid/pid.hpp"
extern sp::DBus remote_controller;

extern sp::CAN can1;

extern sp::RM_Motor motor6020_1;

//初始化电机pid控制器以及电机运动数据
//                             dt     kp    ki    kd    mo   mio   alpha  ang? dynamic?
sp::PID motor6020_1_pid_speed(0.001f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, false, true);
MovingData motor6020_1_data;

extern "C" void control_task()
{
  remote_controller.request();  //这里开始的时候要初始化,等待接收第一帧

  //can初始化配置
  can1.config();
  can1.start();
  //遥控器右边拨杆上中下挡控制输入给电机的电压值分别为5.5V, 2.0V, 0V
  //现在我上面的motor_type选择的是GM6020_V,所以cmd函数的输入单位是V

  //电机运动数据初始化

  motor6020_1_data.absolute_speed_set = 0.0f;
  motor6020_1_data.given_torque = 0.0f;
  motor6020_1_data.given_voltage = 0.0f;

  while (true) {
    // 使用调试(f5)查看remote_controller内部变量的变化
    //这里执行遥控器控制任务
    switch (remote_controller.sw_r) {
      case sp::DBusSwitchMode::UP:
        motor6020_1.cmd(5.5f);
        break;
      case sp::DBusSwitchMode::MID:
        motor6020_1.cmd(2.0f);

        //motor6020_1_pid_speed.calc(motor6020_1_data.absolute_speed_set, motor6020_1.speed);
        //motor6020_1_data.given_voltage = motor6020_1_pid_speed.out;

        break;
        //约定右down挡时全部电机失能
      case sp::DBusSwitchMode::DOWN:
        motor6020_1.cmd(0.0f);
        break;
      default:
        break;
    }
    //motor6020_1.cmd(motor6020_1_data.given_voltage);
    motor6020_1.write(can1.tx_data);
    can1.send(motor6020_1.tx_id);
    osDelay(10);
  }
}
