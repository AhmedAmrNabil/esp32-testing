//***********************************************************************************************
//  This example shows how to use the basic methods of the FluxGarage Robo Eyes library.
//
//  Hardware: You'll need a breadboard, a microcontroller such as arduino nano v3 or better an esp32,
//  an I2C oled display with sh1106 chip and some jumper wires.
//
//  Use the dedicated I2C pins of your microcontroller, on ESP32-WROOM-32 module use pin 22 for SCL and pin 21 for SDA.
//  Please note: This example turned out to have slow refresh rates on Arduino Uno. Did not find a fix yet.
//
//  Published in January 2025 by Dennis Hoelscher, FluxGarage
//  www.youtube.com/@FluxGarage
//  www.fluxgarage.com
//
//***********************************************************************************************

#include <SPI.h>
#include <TFT_eSPI.h>

#define SCREEN_WIDTH tft.width()    // OLED display width, in pixels
#define SCREEN_HEIGHT tft.height()  // OLED display height, in pixels

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);  // Create a sprite linked to TFT
#include "FluxGarage_RoboEyes.h"

RoboEyes roboEyes;  // create RoboEyes instance

void setup() {
    Serial.begin(115200);

    delay(250);  // wait for the OLED to power up
    tft.init();
    tft.setRotation(1);
    sprite.createSprite(240, 135);
    // Startup robo eyes
    roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);  // screen-width, screen-height, max framerate

    // Define some automated eyes behaviour
    roboEyes.setAutoblinker(ON, 5, 2);  // Start auto blinker animation cycle -> bool active, int interval, int variation -> turn on/off, set interval between each blink in full seconds, set range for random interval variation in full seconds
    roboEyes.setIdleMode(ON, 10, 2);    // Start idle animation cycle (eyes looking in random directions) -> turn on/off, set interval between each eye repositioning in full seconds, set range for random time interval variation in full seconds

    // Define eye shapes, all values in pixels
    roboEyes.setWidth(38, 38);   // byte leftEye, byte rightEye
    roboEyes.setHeight(52, 52);  // byte leftEye, byte rightEye
    // roboEyes.setBorderradius(8, 8); // byte leftEye, byte rightEye
    roboEyes.setSpacebetween(15);  // int space -> can also be negative

    // Cyclops mode
    // roboEyes.setCyclops(ON); // bool on/off -> if turned on, robot has only on eye

    // Define mood, curiosity and position
    roboEyes.setMood(ROBO_DEFAULT);  // mood expressions, can be TIRED, ANGRY, HAPPY, DEFAULT
    // roboEyes.setPosition(DEFAULT); // cardinal directions, can be N, NE, E, SE, S, SW, W, NW, DEFAULT (default = horizontally and vertically centered)
    roboEyes.setCuriosity(ON);  // bool on/off -> when turned on, height of the outer eyes increases when moving to the very left or very right

    // Set horizontal or vertical flickering
    // roboEyes.setHFlicker(ON, 2); // bool on/off, byte amplitude -> horizontal flicker: alternately displacing the eyes in the defined amplitude in pixels
    // roboEyes.setVFlicker(ON, 2); // bool on/off, byte amplitude -> vertical flicker: alternately displacing the eyes in the defined amplitude in pixels

    // Play prebuilt oneshot animations
    // roboEyes.anim_confused();  // confused - eyes shaking left and right
    // roboEyes.anim_laugh(); // laughing - eyes shaking up and down

}  // end of setup

void loop() {
    roboEyes.update();  // update eyes drawings
                        // Dont' use delay() here in order to ensure fluid eyes animations.
                        // Check the AnimationSequences example for common practices.
}
