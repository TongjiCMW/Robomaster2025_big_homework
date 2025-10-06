
#include "cmsis_os.h"
#include "io/dbus/dbus.hpp"
#include "io/plotter/plotter.hpp"
#include "motor.hpp"
#include "motor/rm_motor/rm_motor.hpp"
#include "motor/super_cap/super_cap.hpp"
#include "power_calculate.hpp"
#include "referee/pm02/pm02.hpp"
extern sp::DBus remote_controller;
extern sp::RM_Motor motor3508_1;
extern sp::RM_Motor motor3508_2;
extern sp::RM_Motor motor3508_3;
extern sp::RM_Motor motor3508_4;
extern MovingData motor3508_1_data;
extern MovingData motor3508_2_data;
extern MovingData motor3508_3_data;
extern MovingData motor3508_4_data;
extern sp::PM02 pm02;
extern sp::SuperCap supercap;

sp::Plotter plotter(&huart1);

extern PowerControl chassis_power_control;

extern "C" void plotter_task()
{
  while (true) {
    plotter.plot(
      supercap.power_in - supercap.power_out, chassis_power_control.power_prediction,
      pm02.robot_status.chassis_power_limit + 20.0f);
    //这个20是电容策略的一个值,表示允许的功率阈值比裁判系统给出的功率高20W
    //plotter.plot(motor3508_1.speed, motor3508_1_data.absolute_speed_set);
    //plotter.plot(remote_controller.ch_lh, remote_controller.ch_lv);
    // if (motor3508_1.is_alive(osKernelSysTick())) {
    //   plotter.plot(1.0f);
    // }
    // else {
    //   plotter.plot(0.0f);
    // }
    osDelay(10);  // 100Hz
  }
}