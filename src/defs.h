#ifndef DEFS_H
#define DEFS_H

#define SW TX2
#define CLK D4
#define DT RX2
#define BUZZER_PIN D15
#define BUZZER_CH 0
#define PWM_FREQ 2000     // 2kHz (matches many active buzzers)
#define PWM_RESOLUTION 8  // 8-bit resolution (0–255)
#define TFT_BG_CLR 0xfb00
#define MAIN_CLR TFT_WHITE
#define ALT_CLR TFT_WHITE
#define SUCESS_CLR TFT_WHITE
#define HOLD_THRESHOLD 200  // ms

#endif  // DEFS_H