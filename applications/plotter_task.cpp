
#include "cmsis_os.h"
#include "io/dbus/dbus.hpp"
#include "io/plotter/plotter.hpp"
#include "motor/rm_motor/rm_motor.hpp"

extern sp::DBus remote_controller;
extern sp::RM_Motor motor3508_1;
extern sp::RM_Motor motor3508_2;
extern sp::RM_Motor motor3508_3;
extern sp::RM_Motor motor3508_4;
sp::Plotter plotter(&huart1);

extern "C" void plotter_task()
{
  while (true) {
    //plotter.plot(motor3508_1.speed, motor3508_2.speed, motor3508_3.speed, motor3508_4.speed);
    plotter.plot(remote_controller.ch_lh, remote_controller.ch_lv);
    osDelay(10);  // 100Hz
  }
}