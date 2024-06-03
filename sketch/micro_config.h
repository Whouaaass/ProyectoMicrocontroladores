#ifndef MICRO_CONFIG_H
#define MICRO_CONFIG_H

#include <LiquidCrystal.h>
#include <Keypad.h>
#include <DHT.h>
#include "Button.h"

#define DHT_TYPE DHT11
#define DHT_PIN 39
#define PHOTOCELL_PIN A0
#define BUZZER_PIN 7
#define HALL_PIN A1
#define LCD_RS 12
#define LCD_EN 11
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2
#define LED_R 8
#define LED_G 9
#define LED_B 10
#define BUTTON_PIN 53

/* CONFIGURACION LCD */
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

/* CONFIGURACION DHT */
DHT dht(DHT_PIN, DHT_TYPE);

/* CONFIGURACION KEYPAD */
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
char padKeys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte keypadRowPins[KEYPAD_ROWS] = {22, 24, 26, 28}; //connect to the row pinouts of the keypad
byte keypadColPins[KEYPAD_COLS] = {30, 32, 34, 36}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(padKeys), keypadRowPins, keypadColPins, KEYPAD_ROWS, KEYPAD_COLS);

/* CONFIGURACIÓN BOTON */
Button button(BUTTON_PIN, true);

#endif