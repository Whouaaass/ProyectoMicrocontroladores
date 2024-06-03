/**
 * @file tasks_config.h
 * @author frdy
 * @brief Archivo de configuración de las tareas asincrónicas
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef TASK_CONFIG_H
#define TASK_CONFIG_H

#include "AsyncTaskLib.h"
#include "variables.h"
#include "menu_config.h"

/*!
 * @brief función que actualiza el valor de la temperatura
 * 
 */
void controlTemperatura();
/**
 * @brief función que actualiza el valor la humedad
 * 
 */
void controlHumedad();
/**
 * @brief función que actualiza el valor del sensor fotoresistor
 * 
 */
void controlLuz();
/**
 * @brief función que actualiza el valor del sensor hall
 * 
 */
void controlHall();
/**
 * @brief función que define el comportamiento cuando el usuario pasa tiempo sin interactuar con el sistema de seguridad
 * 
 */
void onSeguridadAFK();
/**
 * @brief función auxiliar para poner el valor de entrada de Timeout en la variable "input"
 * 
 */
void inputTimeout();
/**
 * @brief función que actualiza el menú instanciado de la librería LiquidMenu
 * 
 */
void onUpdateMenu();

AsyncTask taskUpdateMenu(1000, true, onUpdateMenu);
AsyncTask taskTemperatura(1000, true, controlTemperatura);
AsyncTask taskHumedad(1000, true, controlHumedad);
AsyncTask taskLuz(1000, true, controlLuz);
AsyncTask taskHall(1000, true, controlHall);
AsyncTask taskTimeoutEventos(3000, false, inputTimeout);
AsyncTask taskTimeoutAmbiental(7000, false, inputTimeout);
AsyncTask taskTimeoutAlarma(4000, false, inputTimeout);
AsyncTask taskTimeoutInicioAFK(7000, false, onSeguridadAFK);


void controlTemperatura() {
  T = temperatura.rolling(dht.readTemperature());
  //mainMenu.update();
  if (T > tmp_high) {
    strcpy(messageAlarma, "'Ta Caliente");
    input = Input::Umbral;
  }
}

void controlHumedad() {
  H = humedad.rolling(dht.readHumidity());
  //mainMenu.update();
}

void controlLuz() {
  L = luz.rolling(analogRead(PHOTOCELL_PIN));
  //mainMenu.update();
  if (L > luz_high) {
    strcpy(messageAlarma, "'Ta Oscuro");
    input = Input::Umbral;
  }
}

void controlHall() {
  HALL = hall.rolling(analogRead(HALL_PIN));
  //mainMenu.update();
  if (HALL > hall_high) {
    strcpy(messageAlarma, "Hay Iman");
    input = Input::Umbral;
  }
}

void onSeguridadAFK() {
  --password_attempts;  
  keypadBuffer.clear();
  setLED(1, 1, 0);  
  lcd.clear();
  lcd.print("Incorrecto");
  delay(1000);
  setLED(0, 0, 0);  
  lcd.clear();
  lcd.print("Clave: ");  
}

void inputTimeout() {
  input = Timeout;
}

void onUpdateMenu() {
  mainMenu.update();
}

#endif