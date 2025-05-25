#ifndef COMMON_h
#define COMMON_h

#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern TFT_eSprite sprite;
static uint16_t mapU8g2ColorToTFT(uint8_t color) {
    switch (color) {
        case 0:              // transparent / no draw
            return TFT_BLACK;  // special code for "do nothing" or skip drawing
        case 1:              // black
            return TFT_CYAN;
        default:
            return TFT_BLACK;  // fallback
    }
}

#endif