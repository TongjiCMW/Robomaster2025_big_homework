#ifndef BUZZER_TASK_HPP
#define BUZZER_TASK_HPP

#ifdef __cplusplus
extern "C" {
#endif

// 上电成功提示音
void power_on_beep();

// 报错音调1 - 超级马里奥死亡音效 (下行音阶，表示严重错误)
void error_sound_mario_death();

// 报错音调2 - 帝国进行曲 (威严音调，表示系统错误)
void error_sound_imperial_march();

void error_sound_lemon_intro(void);

#ifdef __cplusplus
}
#endif

#endif  // BUZZER_TASK_HPP
