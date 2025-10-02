#include "cmsis_os.h"
#include "io/can/can.hpp"
#include "io/dbus/dbus.hpp"
#include "motor/rm_motor/rm_motor.hpp"
//这个文件初始化了所有通信模块(包括通信模块连接的外设如电机)
//其他文件调用对应class时候只需要extern就行

// C板
sp::DBus remote_controller(&huart3);
//这个是遥控器的实例化,名字是remote_controller
// 达妙
// sp::DBus remote(&huart5, false);

// CAN1总线实例化
sp::CAN can1(&hcan1);

// 电机实例化
sp::RM_Motor motor6020_1(1, sp::RM_Motors::GM6020_V);
/*
sp::RM_Motor motor3508_1(1, sp::RM_Motors::RM3508, 14.9f);
a. 麦轮底盘数据：⻨轮直径154mm，⻨轮横向间距370mm，纵向间距330mm。
b. 麦轮底盘四个电机均采用减速比为14.9的RM3508电机，电机id请自行查看电调绿灯灯效，结
合C620电调说明书和电机说明书查询。
*/
extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t Size)
{
  auto stamp_ms = osKernelSysTick();

  if (huart == &huart3) {
    remote_controller.update(Size, stamp_ms);
    remote_controller.request();
  }  //这个是遥控器发送数据,c板接收  先解析上一帧再准备接收下一帧
}

extern "C" void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart)
{
  if (huart == &huart3) {
    remote_controller.request();
  }
}
//CAN接收中断回调函数 读取电机数据
extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef * hcan)
{
  auto stamp_ms = osKernelSysTick();

  while (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0) {
    if (hcan == &hcan1) {
      can1.recv();

      if (can1.rx_id == motor6020_1.rx_id) motor6020_1.read(can1.rx_data, stamp_ms);
    }
  }
}