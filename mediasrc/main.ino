#include <SPI.h>

#include <TFT_eSPI.h>  // Hardware-specific library
#include <ESP32Encoder.h>
#include "bitmaps.h"

#define ENC_CLK 15
#define ENC_DT 2
#define ENC_SW 4

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

const unsigned char* volumeImage(int volume) {
    if (volume == 0) return image_volume_no_sound_bits;
    if (volume < 20) return image_volume_low_bits;
    if (volume < 70) return image_volume_normal_bits;
    return image_volume_loud_bits;
}

int vol = 0;

ESP32Encoder encoder;
int oldVol = 0;

void setup(void) {
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);
    encoder.attachHalfQuad(ENC_DT, ENC_CLK);
    encoder.setCount(0);
    tft.drawBitmap(210, 11, volumeImage(vol), 20, 16, 0xFFFF);
    // Volume label
    tft.setTextColor(0xFFFF);
    tft.setTextSize(1);
    tft.setFreeFont(&FreeSans12pt7b);
    tft.drawString("Volume", 10, 10);
    // Separator
    tft.drawLine(0, 38, 239, 38, 0xD5F);
}

void loop() {
    // volume_loud
    vol = encoder.getCount();
    // if (vol == oldVol) return;
    oldVol = vol;
    // tft.fillScreen(TFT_BLACK);

    // Layer 8
    tft.fillRect(75, 70,110,36,TFT_BLACK);
    tft.setTextSize(2);
    char buf[16];
    snprintf(buf, 16, "%d%%", vol);
    tft.drawString(buf, 73, 68);

    delay(100);
}
