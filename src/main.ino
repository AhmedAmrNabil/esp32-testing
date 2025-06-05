#include <TFT_eSPI.h>
#include <ESP32Encoder.h>
#include <ezBuzzer.h>
#include <WiFi.h>
#include <pins.h>
#include <WiFiUdp.h>

#include "defs.h"
#include "credentials.h"
#include "forzaStruct.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
WiFiUDP udp;

unsigned int currentMillis;
const unsigned int localUdpPort = 1151;  // Port to listen on
char incomingPacket[512];                // buffer for incoming packets
TelemetryData teldata{
    .IsRaceOn = 1,
    .EngineMaxRpm = 8000.0f,
    .CurrentEngineRpm = 1000.0f,
    .Speed = 0.0f,
    .Gear = 2,
};
portMUX_TYPE teldataMux = portMUX_INITIALIZER_UNLOCKED;
ESP32Encoder encoder;          // Encoder instance
float redlineThreshold = 0.8;  // RPM threshold for redline color
// =========================================================
void initDisplay();
void initHardware();
void initWifi();
// =========================================================
// Draws the current gear with color based on RPM
// If RPM is above a certain threshold, it will be red, otherwise white
void drawGearColored(TelemetryData teldata);
//=========================================================
// Draws the current speed in km/h at the top of the display
void drawSpeed(TelemetryData teldata);
//=========================================================
// Draws the RPM bar at the bottom of the display
// x, y: position of the bar
void drawRpmBar(TelemetryData teldata, int x, int y, int width, int height);
//=========================================================

void displayTask(void* param) {
    while (true) {
        portENTER_CRITICAL(&teldataMux);
        TelemetryData local = teldata;
        portEXIT_CRITICAL(&teldataMux);
        if (local.EngineMaxRpm > 0 || local.CurrentEngineRpm > 0) {
            sprite.fillSprite(BG_CLR);
            drawSpeed(local);                    // Draw speed in km/h
            drawGearColored(local);              // Draw gear with color based on RPM
            drawRpmBar(local, 1, 240, 239, 30);  // Draw RPM bar
            sprite.pushSprite(0, 0);             // Push the sprite to the TFT display
        }

        vTaskDelay(2 / portTICK_PERIOD_MS);  // ~60fps
    }
}

void setup() {
    Serial.begin(115200);
    initDisplay();
    initHardware();
    initWifi();
    // Create display task pinned to core 0
    xTaskCreatePinnedToCore(
        displayTask,    // Task function
        "DisplayTask",  // Name
        4096,           // Stack siz
        NULL,           // Parameters
        1,              // Priority
        NULL,           // Task handle
        0               // Core 0
    );
    Serial.println("Setup complete, starting loop...");
}

//=========================================================
char gearString[3];
char speedString[10];
unsigned long lastPrint = 0;  // For debugging, to limit print frequency
void loop() {
    currentMillis = millis();
    int packetSize = udp.parsePacket();

    if (packetSize >= 324) {  // Check if packet size is at least the size of TelemetryData struct
        // read the packet into packetBufffer
        portENTER_CRITICAL(&teldataMux);
        TelemetryData teldataTmp;
        udp.read((uint8_t*)&teldataTmp, sizeof(TelemetryData));
        if (teldataTmp.CurrentEngineRpm > 0 && teldataTmp.EngineMaxRpm > 0) {
            teldata = teldataTmp;  // Update global telemetry data
        }
        portEXIT_CRITICAL(&teldataMux);
        if (currentMillis - lastPrint > 500) {  // Print every second
            Serial.printf("Redline RPM: %.2f, Gear: %d, RPM: %.2f, Speed: %.2f\n", teldata.EngineMaxRpm * redlineThreshold, teldata.Gear, teldata.CurrentEngineRpm, teldata.Speed * 3.6);
            lastPrint = currentMillis;
        }
    }
    redlineThreshold = 0.8 + float(encoder.getCount() / 2) * 0.005;  // Adjust redline threshold based on encoder count
    if (redlineThreshold < 0.5) {
        redlineThreshold = 0.5;  // Minimum threshold
    } else if (redlineThreshold > 1.0) {
        redlineThreshold = 1.0;  // Maximum threshold
    }
    delay(1);
}

//=========================================================
void initHardware() {
    // Empty for now
    pinMode(DT, INPUT_PULLUP);   // Encoder DT pin
    pinMode(CLK, INPUT_PULLUP);  // Encoder CLK pin
    pinMode(SW, INPUT_PULLUP);   // Encoder SW pin
    // Initialize the encoder
    encoder.attachHalfQuad(DT, CLK);  // Attach encoder pins
    encoder.setCount(0);              // Initialize encoder count to 0
    Serial.println("Encoder initialized");
}

void initDisplay() {
    tft.init();
#ifdef USE_DMA
    tft.initDMA();
#endif
    tft.setRotation(0);
    tft.fillScreen(BG_CLR);
    tft.setSwapBytes(true);
    sprite.init();
    sprite.setColorDepth(8);
    sprite.createSprite(tft.width(), tft.height());  // Create sprite with 1 frame
    sprite.setSwapBytes(true);
}

void initWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);  // Connect to WiFi using credentials
    while (WiFi.status() != WL_CONNECTED) {
        delay(10);
        Serial.println("Connecting to WiFi...");
        sprite.fillSprite(BG_CLR);
        sprite.setTextSize(1);
        sprite.setTextColor(TFT_WHITE, BG_CLR);
        sprite.setTextDatum(MC_DATUM);
        sprite.setFreeFont(&FreeMonoBold9pt7b);
        sprite.drawString("Connecting to WiFi...", sprite.width() / 2, sprite.height() / 2);
        sprite.pushSprite(0, 0);  // Push the sprite to the TFT display
    }
    udp.begin(localUdpPort);
    Serial.printf("Listening on UDP port %d\n", localUdpPort);
}

void drawGearColored(TelemetryData teldata) {
    sprite.setTextSize(2);
    sprite.setFreeFont(&FreeMonoBold24pt7b);
    sprite.setTextDatum(MC_DATUM);
    if (teldata.CurrentEngineRpm >= teldata.EngineMaxRpm * redlineThreshold) {
        sprite.setTextColor(TFT_RED, BG_CLR);
        sprite.fillCircle(120, 160, 62, TFT_RED);  // Draw a red circle around the gear
    } else {
        sprite.setTextColor(TFT_WHITE, BG_CLR);
        sprite.fillCircle(120, 160, 62, TFT_WHITE);  // Draw a red circle around the gear
    }
    sprite.fillCircle(120, 160, 60, TFT_BLACK);  // Draw a red circle around the gear
    if (teldata.Gear == 0)
        sprite.drawString("R", sprite.width() / 2, sprite.height() / 2);
    else {
        snprintf(gearString, sizeof(gearString), "%d", teldata.Gear);
        sprite.drawString(gearString, sprite.width() / 2 - 3, sprite.height() / 2 - 3);
    }
}

void drawRpmBar(TelemetryData teldata, int x = 1, int y = 1, int width = 0, int height = 0) {
    if (width == 0 || width > sprite.width() - 1) {
        width = sprite.width() - 1;  // Default width if not specified
    }
    if (height == 0) {
        height = 24;  // Default height if not specified
    }
    // y = 220
    // draw frame
    sprite.drawRect(x, y, width, height, 0xFFFF);
    sprite.drawRect(x + 1, y + 1, width - 2, height - 2, 0xFFFF);
    // draw background
    int greyWidth = (width - 4) * redlineThreshold;  // Calculate width for grey section based on redline threshold
    sprite.fillRect(x + 2, y + 2, greyWidth, height - 4, TFT_DARKGREY);
    sprite.fillRect(x + 2 + greyWidth, y + 2, width - 4 - greyWidth, height - 4, 0x8800);  // Red section for RPM above threshold

    int color = 0xFFFF;  // Default color for RPM bar
    int barWidth = (width - 6) * (teldata.CurrentEngineRpm / teldata.EngineMaxRpm);
    sprite.fillRect(x + 3, y + 3, barWidth, height - 6, color);
}

void drawSpeed(TelemetryData teldata) {
    sprite.setTextSize(1);
    sprite.setTextColor(TFT_WHITE, BG_CLR);
    sprite.setCursor(0, 10);
    sprite.setTextDatum(TC_DATUM);
    sprite.setFreeFont(&FreeMonoBoldOblique24pt7b);
    snprintf(speedString, sizeof(speedString), "%03d", int(teldata.Speed * 3.6));  // Convert speed to km/h
    sprite.drawString(speedString, sprite.width() / 2, 30);
}