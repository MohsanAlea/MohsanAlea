#include <Keyboard.h>
#include <Mouse.h>
#include <Keypad.h>

const int xPin = A0; 
const int yPin = A1;  
const int zPin = A2;  
const int joyButtonPin = A4; 
const int ledPin = 13; 

int xValue = 0;
int yValue = 0;
int zValue = 0;
int xKey = 0;
int yKey = 0;
int zKey = 0;
int prevXValue = 0;
int prevYValue = 0;
int prevZValue = 0;
int threshold = 30;
int joyButtonState = 0;

unsigned long lastKeyPressTimePlus = 0;
unsigned long lastKeyPressTimeMinus = 0;
unsigned long lastKeyPressTimeLeft = 0;
unsigned long lastKeyPressTimeRight = 0;
unsigned long lastKeyPressTimeUp = 0;
unsigned long lastKeyPressTimeDown = 0;
unsigned long keyPressInterval = 200;

bool keyPressedPlus = false;
bool keyPressedMinus = false;
bool keyPressedLeft = false;
bool keyPressedRight = false;
bool keyPressedUp = false;
bool keyPressedDown = false;
bool normalState = true;

const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Keyboard.begin();
  Mouse.begin();
  pinMode(joyButtonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

bool debounce(unsigned long &lastKeyPressTime, unsigned long interval) {
  unsigned long currentTime = millis();
  if (currentTime - lastKeyPressTime >= interval) {
    lastKeyPressTime = currentTime;
    return true;
  }
  return false;
}

void checkKeypad() {
    char key = keypad.getKey();
    if (key) {
      switch (key) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      Keyboard.write(key);
      break;
      case '*':
      Keyboard.write(KEY_ESC);
      break;
      case '#':
      Keyboard.write(KEY_RETURN);
      break;
      case 'D':
      Keyboard.write(KEY_BACKSPACE);
      break;
      case 'B':
      Keyboard.write(KEY_HOME);
      break;
      case 'C':
      Keyboard.write(KEY_END);
      break;
      case 'A': {
        normalState = !normalState;
      }
      break;
      default:
      break;
      }
    }
}

void loop() {

    digitalWrite(ledPin, normalState ? HIGH : LOW);  
    xValue = analogRead(xPin);
    yValue = analogRead(yPin);
    zValue = analogRead(zPin);
    joyButtonState = digitalRead(joyButtonPin);
 
 checkKeypad();
 
   if (normalState) {
 
    int mouseX = map(xValue, 0, 1023, -10, 10);
    int mouseY = map(yValue, 1020, 0, -10, 10);
    if (yValue > 512 && yValue < 518) {
      mouseY = 0;
    }

    Mouse.move(mouseX, mouseY, 0);

    if (joyButtonState == LOW) {
      Mouse.press(MOUSE_LEFT);
      delay(100);
      Mouse.release(MOUSE_LEFT);
    }

    if (zValue >= 0 && zValue <= 60) {
      Mouse.press(MOUSE_RIGHT);
      delay(100);
      Mouse.release(MOUSE_RIGHT);
    }

    if (zValue >= 1000 && zValue <= 1023) {
      Mouse.press(MOUSE_LEFT);
      delay(100);
      Mouse.release(MOUSE_LEFT);
    }

    delay(10); // تاخیر کوتاه برای استقرار
  
   } else {
    if (abs(xValue - prevXValue) >= threshold) {
      xKey = map(xValue, 0, 990, -10, 10);
      if (xKey != 0) {
        if (xKey < 0) {
          if (debounce(lastKeyPressTimeLeft, keyPressInterval)) {
            Keyboard.press(KEY_LEFT_ARROW);
            keyPressedLeft = true;
          }
        } else {
          if (debounce(lastKeyPressTimeRight, keyPressInterval)) {
            Keyboard.press(KEY_RIGHT_ARROW);
            keyPressedRight = true;
          }
        }
      } else {
        if (keyPressedLeft) {
          Keyboard.release(KEY_LEFT_ARROW);
          keyPressedLeft = false;
        }
        if (keyPressedRight) {
          Keyboard.release(KEY_RIGHT_ARROW);
          keyPressedRight = false;
        }
      }
      prevXValue = xValue;
    }
    if (abs(yValue - prevYValue) >= threshold) {
      yKey = map(yValue, 0, 985, -10, 10);
      if (yKey != 0) {
        if (yKey > 0) {
          if (debounce(lastKeyPressTimeUp, keyPressInterval)) {
            Keyboard.press(KEY_UP_ARROW);
            keyPressedUp = true;
          }
        } else {
          if (debounce(lastKeyPressTimeDown, keyPressInterval)) {
            Keyboard.press(KEY_DOWN_ARROW);
            keyPressedDown = true;
          }
        }
      } else {
        if (keyPressedUp) {
          Keyboard.release(KEY_UP_ARROW);
          keyPressedUp = false;
        }
        if (keyPressedDown) {
          Keyboard.release(KEY_DOWN_ARROW);
          keyPressedDown = false;
        }
      }
      prevYValue = yValue;
    }
    if (zValue >= 1013 && zValue <= 1023) {
      if (debounce(lastKeyPressTimePlus, keyPressInterval)) {
        Keyboard.press(KEY_KP_PLUS);
        keyPressedPlus = true;
      }
    } else {
      if (keyPressedPlus) {
        Keyboard.release(KEY_KP_PLUS);
        keyPressedPlus = false;
      }
    }
    if (zValue >= 0 && zValue <= 10) {
      if (debounce(lastKeyPressTimeMinus, keyPressInterval)) {
        Keyboard.press(KEY_KP_MINUS);
        keyPressedMinus = true;
      }
    } else {
      if (keyPressedMinus) {
        Keyboard.release(KEY_KP_MINUS);
        keyPressedMinus = false;
      }
    }
  }
}
