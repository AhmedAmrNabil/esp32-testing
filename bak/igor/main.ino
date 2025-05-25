#include <TFT_eSPI.h>
#include <ESP32Encoder.h>
#include <pins.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);  // Create sprite linked to tft

#define SW TX2
#define CLK D4
#define DT RX2
#define BUZZER_PIN D25
#define BUZZER_CH 0
#define PWM_FREQ 2000     // 2kHz (matches many active buzzers)
#define PWM_RESOLUTION 8  // 8-bit resolution (0–255)
#define TFT_BG_CLR 0xfb00
#define MAIN_CLR TFT_BLACK
#define ALT_CLR TFT_BLACK
#define SUCESS_CLR TFT_BLACK

//-----------------------------------------------
int flowMinutes = 0;                              // Total flow minutes
int menuIndex = 0;                                // 0 for UP, 1 for DOWN, 2 for Reset
String menuOptions[3] = {"UP", "DOWN", "Reset"};  // Label reset option as "Reset"
unsigned long lastActivityTime = 0;               // For inactivity detection
const unsigned long inactivityLimit = 3 * 60000;  // 3 minutes in milliseconds

enum State { MENU,
             COUNTING_UP,
             COUNTING_DOWN,
             SELECTING_DOWN_DURATION,
             IDLE };
State currentState = MENU;

int countdownValue = 20;           // Default value for countdown
int initialCountdownValue = 20;    // Store the countdown value when selected
unsigned long previousMillis = 0;  // For counting logic
int elapsedSeconds = 0;
bool isCounting = false;
unsigned long buttonDebounceTime = 0;
const unsigned long buttonDebounceDelay = 800;  // Debounce delay

// Rotary encoder debounce variables
unsigned long lastRotaryTime = 0;
const unsigned long rotaryDebounceDelay = 150;  // Faster debounce for rotary encoder

// IDLE mode extended behavior
const unsigned long displayOffTimeLimit = 30 * 60000;  // 30 minutes in milliseconds

unsigned long idleStartTime = 0;  // Track when IDLE mode starts
bool displayOff = false;          // Track if the display is off
unsigned long currentMillis;
ESP32Encoder encoder;
int oldCount = 0;

//=========================================================
void setup() {
    initHardware();
    initDisplay();
    updateDisplay();
    Serial.println("Setup complete, starting loop...");
}

//=========================================================
void loop() {
    currentMillis = millis();

    // Handle rotary encoder input
    handleRotaryInput();

    // Handle button presses and states
    handleButtonPresses(currentMillis);

    // Handle counting logic
    handleCounting(currentMillis);

    // Handle inactivity
    // handleInactivity(currentMillis);
}

//=========================================================
// Initialize hardware pins and serial communication
void initHardware() {
    pinMode(CLK, INPUT_PULLUP);
    pinMode(DT, INPUT_PULLUP);
    pinMode(SW, INPUT_PULLUP);
    encoder.attachHalfQuad(DT, CLK);
    encoder.setCount(oldCount);
    ledcSetup(BUZZER_CH, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CH);
    Serial.begin(115200);
}

//=========================================================
// Initialize the OLED display
void initDisplay() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BG_CLR);
    // Initialize the sprite with TFT dimensions
    sprite.createSprite(tft.width(), tft.height());

    sprite.fillSprite(TFT_BG_CLR);  // Clear sprite
    sprite.pushSprite(0, 0);        // Push empty screen at start
    Serial.println("Display initialized.");
}

//=========================================================
// Update the OLED display with the current state
void updateDisplay() {
    sprite.fillSprite(TFT_BG_CLR);
    // Display top row
    String topRowText;

    if (currentState == COUNTING_UP) {
        // topRowText = "Focus! \x18";  // Focus with upward triangle for counting UP
        topRowText = "Focus!";  // Focus with upward triangle for counting UP
    } else if (currentState == COUNTING_DOWN) {
        // topRowText = "Focus! \x19";  // Focus with downward triangle for counting DOWN
        topRowText = "Focus!";  // Focus with downward triangle for counting DOWN
    } else {
        topRowText = "Flow: " + String(flowMinutes);  // Display total flow minutes when not counting
    }

    sprite.setTextSize(3);  // Larger size for top row
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextColor(MAIN_CLR);
    sprite.drawString(topRowText, tft.width() / 2, sprite.fontHeight() / 2);

    // Display main row (menu or counting values)
    String mainRowText;
    char time[10];

    if (currentState == MENU) {
        mainRowText = menuOptions[menuIndex];  // Display UP, DOWN, or Reset in the menu
    } else if (currentState == COUNTING_UP) {
        int seconds = elapsedSeconds % 60;
        int mins = elapsedSeconds / 60;
        snprintf(time, sizeof(time), "%02d:%02d", mins, seconds);
        mainRowText = String(time);  // Display counting up minutes
    } else if (currentState == COUNTING_DOWN || currentState == SELECTING_DOWN_DURATION) {
        int seconds = countdownValue % 60;
        int mins = countdownValue / 60;
        snprintf(time, sizeof(time), "%02d:%02d", mins, seconds);
        mainRowText = String(time);
    } else if (currentState == IDLE) {
        mainRowText = "IDLE?";
    }

    int mainRowTextWidth = mainRowText.length() * 24;  // TextSize 4, so 24 pixels per char
    int mainRowX = (128 - mainRowTextWidth) / 2;       // Calculate centered X position

    sprite.setTextSize(5);
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextColor(ALT_CLR);
    sprite.drawString(mainRowText, tft.width() / 2, sprite.fontHeight() * 1.6);
    sprite.pushSprite(0, 0);
    // display.display();  // Show the updated display
}

//=========================================================
// Detect button presses with debounce logic
bool buttonPressed() {
    if (digitalRead(SW) == LOW && (millis() - buttonDebounceTime > buttonDebounceDelay)) {
        buttonDebounceTime = millis();  // Debounce
        lastActivityTime = millis();    // Reset inactivity timer
        return true;
    }
    return false;
}

//=========================================================
// Handle button presses and manage state transitions
void handleButtonPresses(unsigned long currentMillis) {
    if (!buttonPressed()) return;

    switch (currentState) {
        case MENU:
            if (menuIndex == 0) {  // UP selected
                startCountingUp();
            } else if (menuIndex == 1) {  // DOWN selected
                startSelectingDownDuration();
            } else if (menuIndex == 2) {  // Reset selected
                resetFlowMinutes();       // Reset the total focus time to 0
            }
            break;

        case SELECTING_DOWN_DURATION:
            confirmCountdownSelection();
            break;

        case COUNTING_UP:
            stopCountingUp();
            break;

        case COUNTING_DOWN:
            stopCountingDown();
            break;
    }
    updateDisplay();
}

//=========================================================
// Start counting up
void startCountingUp() {
    currentState = COUNTING_UP;
    elapsedSeconds = 0;
    isCounting = true;
    lastActivityTime = millis();  // Reset inactivity timer
    Serial.println("Counting UP started.");
}

//=========================================================
// Start selecting the countdown duration
void startSelectingDownDuration() {
    currentState = SELECTING_DOWN_DURATION;
    countdownValue = 25 * 60;
    lastActivityTime = millis();  // Reset inactivity timer
    Serial.println("Selecting DOWN duration.");
}

//=========================================================
// Confirm countdown selection and start counting down
void confirmCountdownSelection() {
    initialCountdownValue = countdownValue;
    currentState = COUNTING_DOWN;
    isCounting = true;
    lastActivityTime = millis();  // Reset inactivity timer
    Serial.print("Counting DOWN started with ");
    Serial.print(countdownValue);
    Serial.println(" minutes.");
}

//=========================================================
// Stop counting up and return to menu
void stopCountingUp() {
    flowMinutes += elapsedSeconds / 60;
    successAnimation();
    currentState = MENU;
    isCounting = false;
    Serial.println("Counting UP stopped. Returning to MENU.");
}

//=========================================================
// Stop counting down and return to menu
void stopCountingDown() {
    flowMinutes += (initialCountdownValue - countdownValue) / 60;
    successAnimation();
    currentState = MENU;
    isCounting = false;
    Serial.println("Counting DOWN stopped. Returning to MENU.");
}

//=========================================================
// Reset the total flow minutes counter to 0
void resetFlowMinutes() {
    flowMinutes = 0;
    Serial.println("Flow minutes reset to 0.");
    updateDisplay();  // Update the display to show the reset value
}

//=========================================================
// Handle counting up or down logic
void handleCounting(unsigned long currentMillis) {
    if (!isCounting || (currentMillis - previousMillis < 1000)) return;

    previousMillis = currentMillis;

    if (currentState == COUNTING_UP) {
        elapsedSeconds++;
        updateDisplay();
        Serial.print("Counting UP: ");
        Serial.println(elapsedSeconds);
    } else if (currentState == COUNTING_DOWN) {
        countdownValue--;
        if (countdownValue <= 0) {
            flowMinutes += initialCountdownValue / 60;
            successAnimation();
            currentState = MENU;
            isCounting = false;
            Serial.println("Countdown finished, returning to MENU.");
        }
        updateDisplay();
        Serial.print("Counting DOWN: ");
        Serial.println(countdownValue);
    }
}

//=========================================================
// Success animation when a session ends
void successAnimation() {
    // display.clearDisplay()
    pulseBuzzer();
    pulseBuzzer();
    idleStartTime = currentMillis;
    sprite.fillSprite(TFT_BG_CLR);
    int centerX = tft.width() / 2, centerY = tft.height() / 2;

    for (int radius = 1; radius <= 72; radius += 1) {
        if (radius % 2 == 0) {
            sprite.fillSprite(TFT_BG_CLR);
            delay(2);
        }
        sprite.drawCircle(centerX, centerY, radius, SUCESS_CLR);
        delay(10);
        sprite.pushSprite(0, 0);
    }

    sprite.fillSprite(TFT_BG_CLR);
    // display.clearDisplay();
    sprite.setTextSize(3);
    sprite.setTextColor(SUCESS_CLR);
    sprite.setTextDatum(CC_DATUM);
    sprite.drawString("SUCESS!", tft.width() / 2, tft.height() / 2);
    sprite.pushSprite(0, 0);
    delay(1000);
    tft.fillScreen(TFT_BG_CLR);
    // display.clearDisplay();
    // display.display();
}

//=========================================================
// Rotary Encoder Rotation Detection
int getRotation() {
    int count = encoder.getCount();
    if (count == oldCount) return 0;
    int output = count > oldCount ? 1 : -1;
    oldCount = count;
    return output;
    // return 0;  // No rotation
}

//=========================================================
// Handle rotary input for menu and countdown selection
void handleRotaryInput() {
    int rotation = getRotation();
    if (rotation == 0) return;  // No rotation detected

    lastActivityTime = millis();  // Reset inactivity timer on any valid rotation
    Serial.print(millis());       // Print the current time in milliseconds
    Serial.print(" - Rotation detected, activity timer reset. Rotation: ");
    Serial.println(rotation);

    if (currentState == MENU) {
        menuIndex = (menuIndex + rotation + 3) % 3;  // Update for 3 menu options: UP, DOWN, Reset
        updateDisplay();
        Serial.print(millis());  // Print the current time in milliseconds
        Serial.print(" - Menu option: ");
        Serial.println(menuOptions[menuIndex]);
    } else if (currentState == SELECTING_DOWN_DURATION) {
        countdownValue = max(1, countdownValue + rotation * 60);
        updateDisplay();
        Serial.print(millis());  // Print the current time in milliseconds
        Serial.print(" - Countdown value: ");
        Serial.println(countdownValue);
    }
}
//=========================================================
// Handle inactivity and switch to IDLE if necessary
void handleInactivity(unsigned long currentMillis) {
    // Comment out frequent serial prints to improve performance
    /*
    Serial.print(millis());
    Serial.print(" - Current time (millis): ");
    Serial.println(currentMillis);

    Serial.print(millis());
    Serial.print(" - Last activity time (millis): ");
    Serial.println(lastActivityTime);
    */

    // Make sure the subtraction does not cause an overflow/underflow
    if (currentMillis >= lastActivityTime) {
        unsigned long timeSinceLastActivity = currentMillis - lastActivityTime;

        /*
        Serial.print(millis());
        Serial.print(" - Time since last activity (ms): ");
        Serial.println(timeSinceLastActivity);
        */

        // Check if the user is in the MENU or selecting countdown duration mode
        if ((currentState == MENU || currentState == SELECTING_DOWN_DURATION) &&
            (timeSinceLastActivity > inactivityLimit)) {
            if (currentState != IDLE) {
                currentState = IDLE;
                idleStartTime = millis();  // Record when IDLE mode starts
                updateDisplay();
                Serial.print(millis());  // Print the current time in milliseconds
                Serial.println(" - IDLE state entered due to inactivity.");
            }
        }
    } else {
        // Comment out warning to reduce unnecessary serial prints
        // Serial.println(" - Warning: currentMillis is less than lastActivityTime!");
    }

    // Check if the user has been in IDLE for more than 30 minutes and turn off the display
    if (currentState == IDLE && !displayOff && (currentMillis - idleStartTime > displayOffTimeLimit)) {
        displayOff = true;
        tft.writecommand(ST7789_DISPOFF);
        // display.ssd1306_command(SSD1306_DISPLAYOFF);  // Turn off the display
        Serial.print(millis());  // Print the current time in milliseconds
        Serial.println(" - Display turned off after 30 minutes of IDLE.");
    }

    // Exit IDLE if any rotary or button action happens
    if (currentState == IDLE && (getRotation() != 0 || buttonPressed())) {
        currentState = MENU;
        lastActivityTime = millis();  // Reset inactivity timer upon exiting IDLE

        // Turn the display back on if it was off
        if (displayOff) {
            tft.writecommand(ST7789_DISPON);
            // display.ssd1306_command(SSD1306_DISPLAYON);
            displayOff = false;
            Serial.print(millis());  // Print the current time in milliseconds
            Serial.println(" - Display turned back on.");
        }

        updateDisplay();
        Serial.print(millis());  // Print the current time in milliseconds
        Serial.println(" - Exiting IDLE mode. Back to MENU.");
    }
}

void pulseBuzzer() {
    ledcWrite(BUZZER_CH, 200);
    delay(200);
    ledcWrite(BUZZER_CH, 0);
    delay(200);
}
