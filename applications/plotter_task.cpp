
#include "cmsis_os.h"
#include "io/dbus/dbus.hpp"
#include "io/plotter/plotter.hpp"
#include "motor/rm_motor/rm_motor.hpp"

extern sp::DBus remote_controller;
sp::Plotter plotter(&huart1);

extern "C" void plotter_task()
{
  while (true) {
    plotter.plot(remote_controller.ch_lh);
    osDelay(10);  // 100Hz
  }
}