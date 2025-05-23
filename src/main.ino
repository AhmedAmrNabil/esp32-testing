#include <SPI.h>

#include <TFT_eSPI.h>     // Hardware-specific library
#include <TFT_eWidget.h>  // Hardware-specific library
#include <ESP32Encoder.h>
#include "bitmaps.h"

#define ENC_CLK 15
#define ENC_DT 2
#define ENC_SW 4

const unsigned char* volumeImage(int volume) {
    if (volume == 0) return image_volume_no_sound_bits;
    if (volume < 20) return image_volume_low_bits;
    if (volume < 70) return image_volume_normal_bits;
    return image_volume_loud_bits;
}

int vol = 0;

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library
ESP32Encoder encoder;
ButtonWidget btnL = ButtonWidget(&tft);
int oldVol = 0;

void setup(void) {
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);
    encoder.attachHalfQuad(ENC_DT, ENC_CLK);
    encoder.setCount(0);
    // tft.drawBitmap(210, 11, volumeImage(vol), 20, 16, 0xFFFF);
    // Volume label
    tft.setTextColor(0xFFFF);
    tft.setTextSize(2);
    tft.setFreeFont();
    tft.drawString("Volume", 10, 10);
    // Separator
    tft.drawLine(0, 34, 239, 34, 0xD5F);
}

void loop() {
    // volume_loud
    vol = encoder.getCount();
    if (vol < 0) {
        encoder.setCount(0);
        vol = 0;
    } else if (vol > 100) {
        encoder.setCount(100);
        vol = 100;
    }
    if (vol != oldVol) {
        oldVol = vol;
        drawVolume();
        drawVolumeIcon();
    }
}

void drawVolumeIcon() {
    tft.fillRect(211, 9, 20, 16, TFT_BLACK);
    tft.drawBitmap(211, 9, volumeImage(vol), 20, 16, TFT_WHITE);
}

void drawVolume() {
    tft.fillRect(74, 71, 92, 28, TFT_BLACK);  // Clear previous volume
    tft.setTextSize(4);
    char buf[16];
    snprintf(buf, 16, "%d%%", vol);
    int16_t textWidth = tft.textWidth(buf);
    int16_t centerX = 74 + (92 - textWidth) / 2;  // Get center
    tft.drawString(buf, centerX, 71);
}