#include "cmsis_os.h"
#include "io/can/can.hpp"
#include "io/dbus/dbus.hpp"
#include "motor/rm_motor/rm_motor.hpp"
// C板
extern sp::DBus remote_controller;
//这个是遥控器的实例化,名字是remote_controller
// 达妙
// sp::DBus remote(&huart5, false);

// CAN1总线实例化
extern sp::CAN can1;

// 电机实例化
extern sp::RM_Motor motor6020_1;
/*
sp::RM_Motor motor3508_1(1, sp::RM_Motors::RM3508, 14.9f);

a. 麦轮底盘数据：⻨轮直径154mm，⻨轮横向间距370mm，纵向间距330mm。
b. 麦轮底盘四个电机均采用减速比为14.9的RM3508电机，电机id请自行查看电调绿灯灯效，结
合C620电调说明书和电机说明书查询。
*/

extern "C" void control_task()
{
  remote_controller.request();  //这里开始的时候要初始化,等待接收第一帧

  //can初始化配置
  can1.config();
  can1.start();
  //遥控器右边拨杆上中下挡控制输入给电机的电压值分别为5.5V, 2.0V, 0V
  //现在我上面的motor_type选择的是GM6020_V,所以cmd函数的输入单位是V
  while (true) {
    // 使用调试(f5)查看remote_controller内部变量的变化
    //这里执行遥控器控制任务
    switch (remote_controller.sw_r) {
      case sp::DBusSwitchMode::UP:
        motor6020_1.cmd(5.5f);
        break;
      case sp::DBusSwitchMode::MID:
        motor6020_1.cmd(2.1f);
        break;
        //约定右down挡时全部电机失能
      case sp::DBusSwitchMode::DOWN:
        motor6020_1.cmd(0.0f);
        break;
      default:
        break;
    }
    motor6020_1.write(can1.tx_data);
    can1.send(motor6020_1.tx_id);
    osDelay(10);
  }
}
