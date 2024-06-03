/**
 * @file menu_config.h
 * @author frdy
 * @brief Header configuración del Menu instancia de LiquidMenu
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MENU_CONFIG
#define MENU_CONFIG

#include <LiquidMenu.h>
#include "variables.h"

void blankFunction() { return; }

/* CONFIGURACION DE LIQUID MENU*/
LiquidLine line_tmp_high(0, 0, "TEMP_HIGH:", tmp_high);
LiquidLine line_tmp_low (0, 1, "TEMP_LOW: ", tmp_low);
LiquidLine line_luz_high(0, 1, "LUZ_HIGH: ", luz_high);
LiquidLine line_luz_low (0, 1, "LUZ_LOW:  ", luz_low);
LiquidLine line_hall    (0, 1, "HALL:     ", hall_high);

LiquidLine line_reset   (0, 1, "RESET");
//LiquidLine line_left(0, 1, "<");
//LiquidLine line_right(7, 1, ">");
LiquidScreen scrn_config;

LiquidLine status_tmp_m(0, 0, "Luz:", L);
LiquidLine status_hum_m(8, 0, "Hum:", H, "%");
LiquidLine status_luz_m(0, 1, "Temp:", T, "C");
LiquidScreen scrn_monitoreo_ambiental(status_tmp_m, status_hum_m, status_luz_m);

LiquidLine status_hall_m(0, 0, "Hall:", HALL);
LiquidScreen scrn_monitoreo_eventos(status_hall_m);

LiquidLine line_alarma(0, 0, "Precaucion");
LiquidLine status_alarma(0, 1, messageAlarma);

LiquidScreen scrn_alarma(line_alarma, status_alarma);

LiquidMenu mainMenu(lcd);

/**
 * @brief Función que inicializa la configuración del menu de la librería LiquidMenu
 * 
 */
void setupLiquidMenu() {
  scrn_config.add_line(line_tmp_high);
  scrn_config.add_line(line_tmp_low);
  scrn_config.add_line(line_luz_high);
  scrn_config.add_line(line_luz_low);
  scrn_config.add_line(line_hall);
  scrn_config.add_line(line_reset);
  line_tmp_high.attach_function(1, blankFunction);
  line_tmp_low.attach_function(1, blankFunction);
  line_luz_high.attach_function(1, blankFunction);
  line_luz_low.attach_function(1, blankFunction);
  line_hall.attach_function(1, blankFunction);
  line_reset.attach_function(1, blankFunction);
  line_tmp_high.set_decimalPlaces(1);
  line_tmp_low.set_decimalPlaces(1);
  scrn_config.set_displayLineCount(2);
  mainMenu.add_screen(scrn_config);
  mainMenu.add_screen(scrn_monitoreo_eventos);
  mainMenu.add_screen(scrn_monitoreo_ambiental);
  mainMenu.add_screen(scrn_alarma);
}


#endif