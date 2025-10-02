
#include "cmsis_os.h"
#include "io/dbus/dbus.hpp"
#include "io/plotter/plotter.hpp"
#include "motor/rm_motor/rm_motor.hpp"
extern sp::RM_Motor motor6020_1;
extern sp::DBus remote_controller;
sp::Plotter plotter(&huart1);

extern "C" void plotter_task()
{
  while (true) {
    plotter.plot(remote_controller.ch_lh, motor6020_1.speed);
    osDelay(10);  // 100Hz
  }
}