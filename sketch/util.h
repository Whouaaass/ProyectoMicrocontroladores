/**
 * @file util.h
 * @author frdy
 * @brief Header con funciones adicionales que son útiles
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef UTIL_H
#define UTIL_H

void setLED(uint16_t red, uint16_t green, uint16_t blue) {
  digitalWrite(LED_R, red);
  digitalWrite(LED_G, green);
  digitalWrite(LED_B, blue);
}

#endif