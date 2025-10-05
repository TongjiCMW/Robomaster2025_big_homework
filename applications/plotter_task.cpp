
#include "cmsis_os.h"
#include "io/dbus/dbus.hpp"
#include "io/plotter/plotter.hpp"
#include "motor.hpp"
#include "motor/rm_motor/rm_motor.hpp"
#include "motor/super_cap/super_cap.hpp"
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

extern float power_prediction;  //这个是一个全局变量,表示功率的预测值

extern "C" void plotter_task()
{
  while (true) {
    //plotter.plot(motor3508_1.speed, motor3508_2.speed, motor3508_3.speed, motor3508_4.speed);
    plotter.plot(supercap.power_in - supercap.power_out, power_prediction);
    //plotter.plot(motor3508_1.speed, motor3508_1_data.absolute_speed_set);
    //plotter.plot(remote_controller.ch_lh, remote_controller.ch_lv);
    osDelay(10);  // 100Hz
  }
}