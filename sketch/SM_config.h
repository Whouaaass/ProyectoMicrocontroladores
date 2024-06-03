/**
 * @file SM_config.h
 * @author frdy
 * @brief Header que configura la máquina de estados
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef SM_CONFIG_H
#define SM_CONFIG_H

#include "StateMachineLib.h"
#include "variables.h"
#include "tasks_config.h"

/**
 * @brief función que maneja la entrada al estado "Inicio"
 * 
 */
void onEnteringInicio();
/**
 * @brief función que maneja la salida del estado "Inicio"
 * 
 */
void onLeavingInicio();
/**
 * @brief función que maneja la entrada al estado "Config"
 * 
 */
void onEnteringConfig();
/**
 * @brief función que maneja la salida del estado "Config"
 * 
 */
void onLeavingConfig();
/**
 * @brief función que maneja la entrada al estado "MonitoreoAmbiental"
 * 
 */
void onEnteringMAmbiental();
/**
 * @brief función que maneja la salida del estado "MonitoreoAmbiental"
 * 
 */
void onLeavingMAmbiental();
/**
 * @brief función que maneja la entrada al estado "MonitoreoEventos"
 * 
 */
void onEnteringMEventos();
/**
 * @brief función que maneja la salida del estado "MonitoreoEventos"
 * 
 */
void onLeavingMEventos();
/**
 * @brief función que maneja la entrada al estado "Bloqueado"
 * 
 */
void onEnteringBloqueado();
/**
 * @brief función que maneja la salida del estado "Bloqueado"
 * 
 */
void onLeavingBloqueado();
/**
 * @brief función que maneja la entrada al estado "Alarma"
 * 
 */
void onEnteringAlarma();
/**
 * @brief función que maneja la salida del estado "Alarma"
 * 
 */
void onLeavingAlarma();

/**
 * @brief Función que controla las comparaciones que tiene la máquina de estados para saber el input
 * resetea el input a Unknown cuando se verifica que hay un Input esperado y la máquina hace una transición
 * 
 * @param inputIn Input perteneciente al lenguaje de la máquina
 * @return Si se recibe un parametro igual al input actual
 */
boolean compareAndResetInput(Input inputIn);

/**
 * @brief Maquina de estados que controla el sistema
 * 
 */
StateMachine stateMachine(6, 12);

/**
 * @brief función que prepara la configuración de la máquina de estados, agregando transiciones y 
 * funciones de entrada y salida de los estados definidos
 */
void setupStateMachine() {
  // Inicio
  stateMachine.AddTransition(Inicio, Config, []() {
    return compareAndResetInput(Input::ClaveCorrecta);
  });
  stateMachine.AddTransition(Inicio, Bloqueado, []() {
    return compareAndResetInput(Input::BloqueoSistema);
  });

  //Config
  stateMachine.AddTransition(Config, MonitoreoAmbiental, []() {
    return compareAndResetInput(BotonPresionado);
  });

  //Monitoreo Ambiental
  stateMachine.AddTransition(MonitoreoAmbiental, Config, []() {
    return compareAndResetInput(BotonPresionado);
  });
  stateMachine.AddTransition(MonitoreoAmbiental, Alarma, []() {
    return compareAndResetInput(Umbral);
  });
  stateMachine.AddTransition(MonitoreoAmbiental, MonitoreoEventos, []() {
    return compareAndResetInput(Timeout);
  });

  // Bloqueado
  stateMachine.AddTransition(Bloqueado, Inicio, []() {
    return compareAndResetInput(Timeout);
  });

  //Monitoreo Eventos
  stateMachine.AddTransition(MonitoreoEventos, Config, []() {
    return compareAndResetInput(BotonPresionado);
  });
  stateMachine.AddTransition(MonitoreoEventos, MonitoreoAmbiental, []() {
    return compareAndResetInput(Timeout);
  });
  stateMachine.AddTransition(MonitoreoEventos, Alarma, []() {
    return compareAndResetInput(Umbral);
  });
  
  //Alarma
  stateMachine.AddTransition(Alarma, MonitoreoAmbiental, []() {
    return compareAndResetInput(Timeout);
  });
  stateMachine.AddTransition(Alarma, Inicio, []() {
    return compareAndResetInput(BotonPresionado);
  });

  stateMachine.SetOnEntering(Inicio, onEnteringInicio);
  stateMachine.SetOnEntering(Config, onEnteringConfig);
  stateMachine.SetOnEntering(MonitoreoAmbiental, onEnteringMAmbiental);
  stateMachine.SetOnEntering(Bloqueado, onEnteringBloqueado);
  stateMachine.SetOnEntering(MonitoreoEventos, onEnteringMEventos);
  stateMachine.SetOnEntering(Alarma, onEnteringAlarma);
  stateMachine.SetOnLeaving(Inicio, onLeavingInicio);
  stateMachine.SetOnLeaving(Config, onLeavingConfig);
  stateMachine.SetOnLeaving(MonitoreoAmbiental, onLeavingMAmbiental);
  stateMachine.SetOnLeaving(Bloqueado, onLeavingBloqueado);
  stateMachine.SetOnLeaving(MonitoreoEventos, onLeavingMEventos);
  stateMachine.SetOnLeaving(Alarma, onLeavingAlarma);
}

// INPUT FUNCTION
boolean compareAndResetInput(Input inputIn) {
  if (input == inputIn) {
    input = Unknown;
    return true;
  }
  return false;  
}

// ENTERING/LEAVING FUNCTIONS
void onEnteringInicio() {
  Serial.println("Entering Inicio");  
  keypadBuffer.clear();
  password_attempts = PASSWORD_ATTEMPTS;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Clave: ");
}
void onLeavingInicio() {
  Serial.println("Leaving Inicio");
  taskTimeoutInicioAFK.Stop();
}

void onEnteringConfig() {
  Serial.println("Entering Configuracion");  
  mainMenu.change_screen(&scrn_config);
  mainMenu.set_focusedLine(0);
  mainMenu.update();
}
void onLeavingConfig() {
  Serial.println("Leaving Configuracion");
}

void onEnteringMAmbiental() {
  Serial.println("Entering Monitoreo Ambiental");  
  taskTemperatura.Start();
  taskHumedad.Start();
  taskLuz.Start();
  taskHall.Start();
  taskTimeoutAmbiental.Start();
  taskUpdateMenu.Start();
  mainMenu.change_screen(&scrn_monitoreo_ambiental);
  mainMenu.update();
}
void onLeavingMAmbiental() {
  Serial.println("Leaving Monitoreo Ambiental");
  taskTemperatura.Stop();
  taskHumedad.Stop();
  taskLuz.Stop();
  taskHall.Stop();
  taskTimeoutAmbiental.Stop();
  taskUpdateMenu.Stop();
}

void onEnteringMEventos() {
  Serial.println("Entering Monitoreo Eventos");  
  taskTemperatura.Start();
  taskHumedad.Start();
  taskLuz.Start();
  taskHall.Start();
  taskTimeoutEventos.Start();
  taskUpdateMenu.Start();
  mainMenu.change_screen(&scrn_monitoreo_eventos);
  mainMenu.update();
}
void onLeavingMEventos() {
  Serial.println("Leaving Monitoreo Eventos");
  taskTemperatura.Stop();
  taskHumedad.Stop();
  taskLuz.Stop();
  taskHall.Stop();
  taskTimeoutEventos.Stop();
  taskUpdateMenu.Stop();
}

void onEnteringBloqueado() {
  Serial.println("Entering Bloqueado");    
}
void onLeavingBloqueado() {
  Serial.println("Leaving Bloqueado");
  setLED(0, 0, 0);
}

void onEnteringAlarma() {
  Serial.println("Entering Alarma");  
  setLED(0, 0, 1);
  tone(buzzer, 97, 4000);
  taskTemperatura.Start();
  taskHumedad.Start();
  taskLuz.Start();
  taskHall.Start();
  taskTimeoutAlarma.Start();
  mainMenu.change_screen(&scrn_alarma);
  mainMenu.update();
}
void onLeavingAlarma() {
  Serial.println("Leaving Alarma");
  taskTemperatura.Stop();
  taskHumedad.Stop();
  taskLuz.Stop();
  taskHall.Stop();
  noTone(buzzer);
  taskTimeoutAlarma.Stop();
  setLED(0, 0, 0);
}

#endif