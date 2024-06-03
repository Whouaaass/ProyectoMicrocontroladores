/**
 * @file variables.h
 * @author frdy
 * @brief Archivo configuración de las variables globales
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef VARIABLES_H
#define VARIABLES_H

#include "Average.h"
#include "micro_config.h"

#define PASSWORD_ATTEMPTS 3
#define DEF_TMP_HIGH 40
#define DEF_TMP_LOW 10
#define DEF_LUZ_HIGH 650
#define DEF_LUZ_LOW 20
#define DEF_HALL 600

/**
 * @brief Estructura que crea un buffer 
 * Es usado especialmente para controlar el input del Keypad
 */
struct buffer {
  static const int size = 16; /*!< Tamaño máximo del buffer */
  char str[size + 1]; /*!< Arreglo en el que se guarda el buffer */
  byte len = 0; /*!< Variable que controla la longuitud actual del buffer*/
  /**
   * @brief Método para agregar un carácter al buffer 
   * @param chr carácter a agregar
   */
  void push(char chr) {
    if (len == size) return;
    str[len++] = chr;
  }
  /**
   * @brief Método para limpiar el buffer   
   */
  void clear() {
    for (size_t i = 0; i < this->len; i++) {
      str[i] = 0;
    }
    len = 0;
  }
  /**
   * @brief Método para saber si el buffer está lleno
   * 
   * @return true el buffer está lleno
   * @return false el buffer no está lleno, pero puede contener carácteres
   */
  bool isFull() {
    return len == size;
  }
  /**
   * @brief Método para saber cual fué el último carácter introducido al arreglo del buffer
   * 
   * @return char Último carácter introducido
   */
  char lastCharacter() {
    return len == 0 ? 0 : str[len - 1];
  }
} keypadBuffer;
const byte password_len = 4;
const char password[password_len + 1] = "4444";
byte password_attempts;

// Variables de la configuaracion de los eventos
float tmp_high = DEF_TMP_HIGH;
float tmp_low = DEF_TMP_LOW;
int16_t luz_high = DEF_LUZ_HIGH;
int16_t luz_low = DEF_LUZ_LOW;
int16_t hall_high = DEF_HALL;

// Variables de Monitoreo
float T = 0; /*!< Temperatura */
int16_t H = 0; /*!< Humedad */
int16_t L = 0; /*!< Luz */
int16_t HALL = 0; /*!< Hall */
Average<float> temperatura(5);
Average<int16_t> humedad(5);
Average<int16_t> luz(5);
Average<int16_t> hall(5);

char messageAlarma[17];

// Enums de la maquina de estados
enum State {
  Inicio = 0,
  Config = 1,
  MonitoreoAmbiental = 2,
  Bloqueado = 3,
  MonitoreoEventos = 4,
  Alarma = 5
};
enum Input {
  Unknown = 0,
  ClaveCorrecta = 1,
  BotonPresionado = 2,
  BloqueoSistema = 3,
  Timeout = 4,
  Umbral = 5
};
Input input = Unknown;

#endif