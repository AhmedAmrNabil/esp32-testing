/***************************************************
 * ESP32-Eyes, Copyright (c) 2023 Alastair Aitchison, Playful Technology
 * Based on previous work by Luis Llamas (www.luisllamas.es)
 *
 ****************************************************/

// INCLUDES
// Built-in Arduino I2C library
#include <Wire.h>
// Defines all face functionality
#include "Face.h"

// CONSTANTS
const byte joystickPins[] = {25, 26};
const byte blinkPin = 16;
const byte moodPins[] = {12, 13, 15};
const eEmotions moods[] = {eEmotions::Angry, eEmotions::Sad, eEmotions::Surprised};

// GLOBALS
Face *face;

void setup(void) {
    // Create a serial connection
    Serial.begin(115200);
    Serial.println(__FILE__ __DATE__);

    // Configure input pins
    // pinMode(blinkPin, INPUT_PULLUP);

    // Create a new face
    face = new Face(/* screenWidth = */ 240, /* screenHeight = */ 135, /* eyeSize = */ 40);
    // Assign the current expression
    // face->Expression.GoTo_Normal();

    // Assign a weight to each emotion
    face->Behavior.SetEmotion(eEmotions::Normal, 1.0);
    face->Behavior.SetEmotion(eEmotions::Angry, 1.0);
    face->Behavior.SetEmotion(eEmotions::Happy, 3.0);
    face->Behavior.SetEmotion(eEmotions::Sleepy, 6.0);
    face->Behavior.SetEmotion(eEmotions::Glee, 5.0);
    face->Behavior.SetEmotion(eEmotions::Sad, 5.0);
    face->Behavior.SetEmotion(eEmotions::Focused, 1.0);
    face->Behavior.SetEmotion(eEmotions::Awe, 3.0);
    face->Behavior.SetEmotion(eEmotions::Unimpressed, 2.0);
    face->Behavior.SetEmotion(eEmotions::Surprised, 3.0);
    // Automatically switch between behaviours (selecting new behaviour randomly based on the weight assigned to each emotion)
    face->RandomBehavior = true;

    // Automatically blink
    face->RandomBlink = true;
    // Set blink rate
    face->Blink.Timer.SetIntervalMillis(10000);

    // Automatically choose a new random direction to look
    face->RandomLook = true;
}

// float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
//   // Check if x is outside the input range
//   if (x <= in_min) return out_min;
//   if (x >= in_max) return out_max;

//   // Calculate the proportion of x relative to the input range
//   float proportion = (x - in_min) / (in_max - in_min);

//   // Map the proportion to the output range and return the result
//   return (proportion * (out_max - out_min)) + out_min;
// }

void loop() {
    static int lastMoveTime;

    // // To avoid making eyes too twitchy (and to allow time for previous move animation to end),
    // // only recalculate new position every 500ms
    // if(millis() - lastMoveTime > 500) {
    //   int yRaw = analogRead(25);
    //   int xRaw = analogRead(26);
    //   float y = mapFloat(yRaw, 0, 4095, 1.0, -1.0);
    //   float x = mapFloat(xRaw, 0, 4095, -1.0, 1.0);
    //   face->Look.LookAt(x, y);
    //   lastMoveTime = millis();
    // }
    // // Blink when joystick pressed in
    // if(!digitalRead(blinkPin)){
    //   face->DoBlink();
    // }

    // Update the face!
    face->Update();
    Serial.print("running\n");
    delay(10);
}