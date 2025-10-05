#include "cmsis_os.h"
#include "io/can/can.hpp"
#include "io/dbus/dbus.hpp"
#include "motor.hpp"
#include "motor/rm_motor/rm_motor.hpp"
#include "tools/pid/pid.hpp"
extern sp::DBus remote_controller;

extern sp::CAN can2;

extern sp::RM_Motor motor3508_1;
extern sp::RM_Motor motor3508_2;
extern sp::RM_Motor motor3508_3;
extern sp::RM_Motor motor3508_4;
//初始化电机pid控制器以及电机运动数据
float dt_inuse = 0.01f;
float kp_inuse = 0.8f;
float ki_inuse = 0.5f;
float kd_inuse = 0.005f;
float mo_inuse = 2.5f;
float mio_inuse = 1.0f;
float alpha_inuse = 0.01f;
bool ang = false;
bool dynamic = true;
//                             dt        kp        ki        kd        mo        mio       alpha     ang?  dynamic?
sp::PID motor3508_1_pid_speed(
  dt_inuse, kp_inuse, ki_inuse, kd_inuse, mo_inuse, mio_inuse, alpha_inuse, ang, dynamic);
MovingData motor3508_1_data;

sp::PID motor3508_2_pid_speed(
  dt_inuse, kp_inuse, ki_inuse, kd_inuse, mo_inuse, mio_inuse, alpha_inuse, ang, dynamic);
MovingData motor3508_2_data;

sp::PID motor3508_3_pid_speed(
  dt_inuse, kp_inuse, ki_inuse, kd_inuse, mo_inuse, mio_inuse, alpha_inuse, ang, dynamic);
MovingData motor3508_3_data;

sp::PID motor3508_4_pid_speed(
  dt_inuse, kp_inuse, ki_inuse, kd_inuse, mo_inuse, mio_inuse, alpha_inuse, ang, dynamic);
MovingData motor3508_4_data;
float temp_speed;
float max_rotate_speed = 6.0f * 3.14159f;  //假设遥控器输入拉满的时候,对应转速为6PI rad/s

// 静止检测相关变量
uint32_t static_start_time = 0;
const uint32_t STATIC_THRESHOLD_MS = 200;  // 长达0.2秒收到静止命令
bool want_static = false;
const float DEAD_ZONE = 0.01f;
// 静止检测函数
void check_static_and_stop_motors()
{
  uint32_t current_time = osKernelSysTick();
  if (
    remote_controller.ch_lv <= DEAD_ZONE && remote_controller.ch_lv >= -DEAD_ZONE &&
    remote_controller.ch_lh <= DEAD_ZONE && remote_controller.ch_lh >= -DEAD_ZONE &&
    remote_controller.ch_rh <= DEAD_ZONE && remote_controller.ch_rh >= -DEAD_ZONE) {
    want_static = true;
  }
  else {
    want_static = false;
  }
  if (want_static) {
    if (static_start_time == 0) {
      static_start_time = current_time;  // 开始计时
    }
    else if (current_time - static_start_time >= STATIC_THRESHOLD_MS) {
      // 执行静止命令
      motor3508_1.cmd(0.0f);
      motor3508_2.cmd(0.0f);
      motor3508_3.cmd(0.0f);
      motor3508_4.cmd(0.0f);
    }
  }
  else {
    // 收到正常指令，重置状态
    static_start_time = 0;
  }
}

extern "C" void control_task()
{
  remote_controller.request();  //这里开始的时候要初始化,等待接收第一帧

  //can初始化配置
  can2.config();
  can2.start();

  //电机运动数据初始化

  motor3508_1_data.given_torque = 0.0f;
  motor3508_2_data.given_torque = 0.0f;
  motor3508_3_data.given_torque = 0.0f;
  motor3508_4_data.given_torque = 0.0f;

  temp_speed = 10.0f;

  while (true) {
    // 使用调试(f5)查看remote_controller内部变量的变化
    //这里执行遥控器控制任务

    //首先我们解算遥控器的输入对应的麦轮线速度
    //假设遥控器输入拉满的时候,对应转速为6PI rad/s
    //其中麦轮的半径r = 0.077m

    //                              前后                            左右                          旋转
    motor3508_1_data.absolute_speed_set = max_rotate_speed * remote_controller.ch_lv +
                                          max_rotate_speed * remote_controller.ch_lh +
                                          max_rotate_speed * remote_controller.ch_rh;
    motor3508_2_data.absolute_speed_set = max_rotate_speed * -remote_controller.ch_lv +
                                          max_rotate_speed * remote_controller.ch_lh +
                                          max_rotate_speed * remote_controller.ch_rh;

    motor3508_3_data.absolute_speed_set = max_rotate_speed * -remote_controller.ch_lv +
                                          max_rotate_speed * -remote_controller.ch_lh +
                                          max_rotate_speed * remote_controller.ch_rh;
    motor3508_4_data.absolute_speed_set = max_rotate_speed * remote_controller.ch_lv +
                                          max_rotate_speed * -remote_controller.ch_lh +
                                          max_rotate_speed * remote_controller.ch_rh;

    switch (remote_controller.sw_r) {
      case sp::DBusSwitchMode::UP:
        //这里之后会写成电容使用策略
        break;

      case sp::DBusSwitchMode::MID:

        motor3508_1_pid_speed.calc(motor3508_1_data.absolute_speed_set, motor3508_1.speed);
        motor3508_1_data.given_torque = motor3508_1_pid_speed.out;
        motor3508_1.cmd(motor3508_1_data.given_torque);

        motor3508_2_pid_speed.calc(motor3508_2_data.absolute_speed_set, motor3508_2.speed);
        motor3508_2_data.given_torque = motor3508_2_pid_speed.out;
        motor3508_2.cmd(motor3508_2_data.given_torque);

        motor3508_3_pid_speed.calc(motor3508_3_data.absolute_speed_set, motor3508_3.speed);
        motor3508_3_data.given_torque = motor3508_3_pid_speed.out;
        motor3508_3.cmd(motor3508_3_data.given_torque);

        motor3508_4_pid_speed.calc(motor3508_4_data.absolute_speed_set, motor3508_4.speed);
        motor3508_4_data.given_torque = motor3508_4_pid_speed.out;
        motor3508_4.cmd(motor3508_4_data.given_torque);
        break;

        //约定右down挡时全部电机失能
        //这里一定是失能而不应该是急刹车
      case sp::DBusSwitchMode::DOWN:
        motor3508_1_data.given_torque = 0.0f;
        motor3508_1.cmd(motor3508_1_data.given_torque);
        motor3508_2_data.given_torque = 0.0f;
        motor3508_2.cmd(motor3508_2_data.given_torque);
        motor3508_3_data.given_torque = 0.0f;
        motor3508_3.cmd(motor3508_3_data.given_torque);
        motor3508_4_data.given_torque = 0.0f;
        motor3508_4.cmd(motor3508_4_data.given_torque);
        break;
      default:
        break;
    }

    // 调用静止检测函数
    check_static_and_stop_motors();

    motor3508_1.write(can2.tx_data);
    motor3508_2.write(can2.tx_data);
    motor3508_3.write(can2.tx_data);
    motor3508_4.write(can2.tx_data);

    can2.send(motor3508_1.tx_id);  //这里1-4电机直接用0x200的id发送
    osDelay(10);
  }
}
