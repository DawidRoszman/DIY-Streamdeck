#include "header.h"
#include <Arduino.h>
#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BleKeyboard.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
BleKeyboard bleKeyboard("DIY Stream Deck");

const int ROW_NUM = 4;
const int COLUMN_NUM = 4;

char keys[ROW_NUM][COLUMN_NUM] = {
    {KEY_F1, KEY_F2, KEY_F3, 'A'},
    {KEY_F4, KEY_F5, KEY_F6, 'B'},
    {KEY_F7, KEY_F8, KEY_F9, 'C'},
    {KEY_F10, KEY_F11, KEY_F12, 'D'}};

char numKeys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

char currentMode = 'A';
bool displayedConnectedMessage = false;

byte pin_rows[ROW_NUM] = {27, 14, 12, 13};
byte pin_column[COLUMN_NUM] = {32, 33, 25, 26};

Keypad modeA = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
Keypad modeD = Keypad(makeKeymap(numKeys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
Keypad keypad = modeA; // default mode

void setup()
{
  Serial.begin(9600);
  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    while (true)
      ;
  }
  bleKeyboard.begin();
  delay(2000);         // wait for initializing
  oled.clearDisplay(); // clear display

  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 25);
  oled.setTextWrap(true);
  oled.println("Waiting for bluetooth connection...");
  oled.display();
}

void loop()
{
  if (bleKeyboard.isConnected())
  {
    if (!displayedConnectedMessage)
    {
      displayConnectedMessage();
      displayedConnectedMessage = true;
      delay(1000);
    }
    displayMode(currentMode);
    char key = keypad.getKey();
    if (key)
    {
      switch (key)
      {
      case 'A':
        currentMode = 'A';
        keypad = modeA;
        break;
      case 'B':
        currentMode = 'B';
        keypad = modeA;
        break;
      case 'C':
        currentMode = 'C';
        keypad = modeA;
        break;
      case 'D':
        currentMode = 'D';
        keypad = modeD;
        break;
      default:
        if (currentMode == 'B')
        {
          sendCtrlAltShiftKey(key);
          break;
        }
        if (currentMode == 'C')
        {
          sendCtrlAltKey(key);
          break;
        }
        sendKey(key);
        break;
      }
    }
  }
  else
  {
    if (displayedConnectedMessage)
    {
      displayedConnectedMessage = false;
      oled.clearDisplay();
      oled.setTextSize(1);
      oled.setCursor(0, 25);
      oled.println("Waiting for bluetooth connection...");
      oled.display();
    }
  }
}

void displayMode(char mode)
{
  if (millis() % 300 == 0)
  {
    oled.clearDisplay();
    oled.setCursor(0, 10);
    oled.setTextSize(3);
    oled.printf("Mode: %c\n", mode);
    oled.display();
  }
}

void displayConnectedMessage()
{
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setCursor(0, 25);
  oled.println("Connected");
  oled.display();
}

void sendKey(char key)
{
  bleKeyboard.write(key);
  delay(100);
}
void sendCtrlAltShiftKey(char key)
{
  bleKeyboard.press(KEY_LEFT_CTRL);
  bleKeyboard.press(KEY_LEFT_ALT);
  bleKeyboard.press(KEY_LEFT_SHIFT);
  bleKeyboard.press(key);
  delay(100);
  bleKeyboard.releaseAll();
}
void sendCtrlAltKey(char key)
{
  bleKeyboard.press(KEY_LEFT_CTRL);
  bleKeyboard.press(KEY_LEFT_ALT);
  bleKeyboard.press(key);
  delay(100);
  bleKeyboard.releaseAll();
}
