#ifndef SM_CONFIG_H
#define SM_CONFIG_H

#include "StateMachineLib.h"
#include "variables.h"
#include "tasks_config.h"

void onEnteringInicio();
void onLeavingInicio();
void onEnteringConfig();
void onLeavingConfig();
void onEnteringMAmbiental();
void onLeavingMAmbiental();
void onEnteringMEventos();
void onLeavingMEventos();
void onEnteringBloqueado();
void onLeavingBloqueado();
void onEnteringAlarma();
void onLeavingAlarma();

boolean compareAndResetInput(Input inputIn);

StateMachine stateMachine(6, 12);

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