#include "buzzer_task.hpp"

#include "cmsis_os.h"
#include "io/buzzer/buzzer.hpp"
#include "io/dbus/dbus.hpp"
#include "motor/rm_motor/rm_motor.hpp"
#include "motor/super_cap/super_cap.hpp"

// C板
sp::Buzzer buzzer(&htim4, TIM_CHANNEL_3, 84e6);
// 达妙
// sp::Buzzer buzzer(&htim12, TIM_CHANNEL_2, 240e6);

//下面是用来检测报错的,电机报错,通信报错,电容报错等
extern sp::DBus remote_controller;
extern sp::RM_Motor motor3508_1;
extern sp::RM_Motor motor3508_2;
extern sp::RM_Motor motor3508_3;
extern sp::RM_Motor motor3508_4;
extern sp::SuperCap supercap;
// 播放单个音符的函数
void play_note(float frequency, uint16_t duration_ms, float duty = 0.2)
{
  buzzer.set(frequency, duty);
  buzzer.start();
  osDelay(duration_ms);
  buzzer.stop();
}

// 上电成功提示音 - 简单的"滴"声
void power_on_beep()
{
  play_note(2000, 200);  // 2kHz, 200ms
  osDelay(50);
}

// 报错音调1 - "超级马里奥"死亡音效 对应电机报错
//先响x声(对应id),然后响马里奥死亡音效
void error_sound_mario_death()
{
  // 经典的马里奥死亡音效：下行音阶
  play_note(659, 150);  // E5
  osDelay(50);
  play_note(622, 150);  // Eb5
  osDelay(50);
  play_note(587, 150);  // D5
  osDelay(50);
  play_note(554, 150);  // C#5
  osDelay(50);
  play_note(523, 150);  // C5
  osDelay(50);
  play_note(494, 150);  // B4
  osDelay(50);
  play_note(466, 150);  // Bb4
  osDelay(50);
  play_note(440, 150);  // A4
  osDelay(50);
  play_note(415, 150);  // Ab4
  osDelay(50);
  play_note(392, 300);  // G4 - 长音结束
}

// 报错音调2 - "帝国进行曲"(星球大战)主题
void error_sound_imperial_march()
{
  // 帝国进行曲的经典开头
  play_note(392, 500);  // G4
  osDelay(50);
  play_note(392, 500);  // G4
  osDelay(50);
  play_note(392, 500);  // G4
  osDelay(50);
  play_note(311, 350);  // Eb4
  osDelay(50);
  play_note(466, 150);  // Bb4
  osDelay(50);
  play_note(392, 500);  // G4
  osDelay(50);
  play_note(311, 350);  // Eb4
  osDelay(50);
  play_note(466, 150);  // Bb4
  osDelay(50);
  play_note(392, 1000);  // G4 - 长音
  osDelay(100);

  // 第二段
  play_note(587, 500);  // D5
  osDelay(50);
  play_note(587, 500);  // D5
  osDelay(50);
  play_note(587, 500);  // D5
  osDelay(50);
  play_note(622, 350);  // Eb5
  osDelay(50);
  play_note(466, 150);  // Bb4
  osDelay(50);
  play_note(370, 500);  // F#4
  osDelay(50);
  play_note(311, 350);  // Eb4
  osDelay(50);
  play_note(466, 150);  // Bb4
  osDelay(50);
  play_note(392, 1000);  // G4 - 结束长音
}

//报错音调3 - 《Lemon》副歌开头
void error_sound_lemon_intro_sound(void)
{
  // 《Lemon》副歌开头 - 高两个八度版本
  play_note(1046, 172);  // 1^ - C
  play_note(1175, 172);  // 2^ - D
  play_note(1318, 172);  // 3^ - E
  play_note(1046, 172);  // 1^ - C
  play_note(880, 172);   // 6 - A
  play_note(880, 259);   // . - 延长
  play_note(880, 172);   // 6 - A
  play_note(1175, 172);  // 2^ - D

  play_note(1976, 172);  // 7^ - B
  play_note(1568, 172);  // 5^ - G
  play_note(1318, 172);  // 3^ - E
  play_note(1318, 259);  // . - 延长
  play_note(1318, 172);  // 3^ - E
  play_note(1976, 172);  // 7^ - B
  play_note(1760, 172);  // 6^ - A

  play_note(1568, 172);  // 5^ - G
  play_note(1046, 172);  // 1^ - C
  play_note(1046, 259);  // . - 延长
  play_note(1046, 172);  // 1^ - C
  play_note(1568, 172);  // 5^ - G
  play_note(1318, 172);  // 3^ - E
  play_note(1318, 259);  // . - 延长
  play_note(1318, 259);  // . - 延长

  /*
play_note(1175, 172);  // 2^ - D

  play_note(1318, 172);  // 3^ - E
  play_note(1397, 172);  // 4^ - F
  play_note(1046, 172);  // 1^ - C
  play_note(1976, 172);  // 7^ - B

  play_note(1046, 172);  // 1^ - C
  play_note(1568, 172);  // 5^ - G
  play_note(1397, 172);  // 4^ - F
  play_note(1318, 172);  // 3^ - E

  play_note(1397, 172);  // 4^ - F
  play_note(1480, 172);  // 4#^ - F#
  play_note(1046, 172);  // 1^ - C
  play_note(1976, 172);  // 7^ - B

  play_note(1760, 172);  // 6^ - A
  play_note(1661, 172);  // 5#^ - G#
  play_note(1661, 259);  // . - 延长
  play_note(1046, 172);  // 1^ - C

  play_note(1175, 172);  // 2^ - D
  play_note(1318, 172);  // 3^ - E
  play_note(1046, 172);  // 1^ - C
  play_note(880, 172);   // 6 - A

  play_note(880, 172);   // 6 - A
  play_note(1175, 172);  // 2^ - D
  play_note(1976, 172);  // 7^ - B
  play_note(1568, 172);  // 5^ - G
  play_note(1318, 172);  // 3^ - E
  play_note(1318, 172);  // 3^ - E
  play_note(1976, 172);  // 7^ - B
  play_note(1760, 172);  // 6^ - A

  play_note(1568, 172);  // 5^ - G
  play_note(1046, 172);  // 1^ - C
  play_note(1046, 172);  // 1^ - C
  play_note(1568, 172);  // 5^ - G
  play_note(1318, 172);  // 3^ - E
  play_note(1318, 259);  // . - 延长
  play_note(1175, 172);  // 2^ - D
  play_note(1318, 172);  // 3^ - E
  play_note(1397, 172);  // 4^ - F
  play_note(1568, 172);  // 5^ - G
  play_note(1397, 172);  // 4^ - F
  play_note(1568, 172);  // 5^ - G
  play_note(1318, 172);  // 3^ - E
  play_note(1568, 172);  // 5^ - G
  play_note(1046, 172);  // 1^ - C
  play_note(1318, 172);  // 3^ - E
  play_note(1175, 172);  // 2^ - D
  play_note(1175, 259);  // . - 延长
  play_note(1175, 172);  // 2^ - D
  play_note(1175, 172);  // 2^ - D
  play_note(1175, 259);  // . - 延长
  play_note(1046, 172);  // 1^ - C
  play_note(1046, 172);  // 1^ - C
  play_note(0, 259);     // . - 延长
  play_note(0, 259);     // . - 延长
  play_note(0, 259);     // . - 延长
  play_note(0, 259);     // . - 延长
  play_note(0, 259);     // . - 延长
  play_note(0, 259);     // . - 延长
*/
}

extern "C" void buzzer_task()
{
  // 上电成功提示音
  power_on_beep();
  //error_sound_mario_death();
  //error_sound_imperial_march();
  //error_sound_lemon_intro_sound();
  while (true) {
    if (!motor3508_1.is_alive(osKernelSysTick())) {
      //power_on_beep();
      power_on_beep();
      error_sound_mario_death();
    }
    if (!motor3508_2.is_alive(osKernelSysTick())) {
      power_on_beep();
      power_on_beep();
      error_sound_mario_death();
    }
    if (!motor3508_3.is_alive(osKernelSysTick())) {
      power_on_beep();
      power_on_beep();
      power_on_beep();
      error_sound_mario_death();
    }
    if (!motor3508_4.is_alive(osKernelSysTick())) {
      power_on_beep();
      power_on_beep();
      power_on_beep();
      power_on_beep();
      error_sound_mario_death();
    }
    if (!supercap.is_alive(osKernelSysTick())) {
      // 超级电容掉线报错音

      error_sound_imperial_march();
    }
    if (!remote_controller.is_alive(osKernelSysTick())) {
      // 遥控器掉线报错音

      error_sound_lemon_intro_sound();
    }
    osDelay(500);
  }
}
