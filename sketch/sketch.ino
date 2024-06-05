/**
 * @file sketch.ino (Main File)
 * @author Fredy Anaya
 * @author Jorge Mera
 * @author David Chacón
 * @brief Archivo principal del proyecto
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "variables.h"
#include "util.h"
#include "Average.h"
#include "melody.h"
#include "SM_config.h"


/* FUNCIONES PREDECLARADAS */
/**
 * @brief función que maneja la seguridad del estado de la maquina "Inicio"
 * Pide una contraseña y verifica si es correcta; también maneja el número de intentos 
 * y la consideración de que el usuario deje la contraseña incompleta
 */
void seguridad();
/**
 * @brief función que maneja el menú del estado de la máquina "Config"
 * Muestra un menú en el que se pueden configurar las variables que ponen los límites
 * cuando se alcanza un registro "peligroso"
 */
void menu();
/**
 * @brief función que maneja el estado de la máquina "Bloqueado"
 * Muestra el comportamiento cuando el sistema está bloqueado 
 */
void bloqueo();
/**
 * @brief función que maneja el estado de la máquina "Alarma"
 * Muestra el mensaje de precaución con el sonido que se hace cuando un monitoreo encuentra
 * una lectura "alta"
 */
void alarma();
/**
 * @brief función que maneja el estado de la máquina "MonitoreoEventos"
 * Monitorea el sensor de HALL 
 */
void monitoreoAmbiental();
/**
 * @brief función que maneja el estado de la máquina "MonitoreoAmbiental"
 * Monitorea los sensores de luz, temperatura y humedad* 
 */
void monitoreoEventos();

/**
 * @brief función que permite cambiar una variable a un paso determinado a través de un caracter utilizado \n
 * es usada para cambiar el valor de las variables en configuración
 * 
 * @tparam T Tipo de dato de la variable a cambiar
 * @param variable Variable a modificar
 * @param step Paso con el que se modificará la variable (adición o substracción)
 * @param key Letra que determinará si se adiciona o substrae
 * @param lower_limit el límite inferior al cual puede llegar la variable
 * @param upper_limit el límite superior al cual puede llegar la variable
 */
template <typename T>
void changeVariableControl(T* variable, T step, char key, T lower_limit, T upper_limit);

/* SETUP ----------------------------------------------------------  */
void setup() {
  // Inicializa los componentes necesarios
  lcd.begin(16, 2);
  dht.begin();
  Serial.begin(9600);

  // Inicializa el LED
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  setLED(0, 0, 0);
  // Inicializa el menu del lcd
  setupLiquidMenu();

  // Inicializa la maquina de estados
  Serial.println("Starting State Machine...");
  setupStateMachine();
  Serial.println("State Machine Started");

  stateMachine.SetState(Inicio, false, true);
}
/* LOOP ----------------------------------------------------------  */
void loop() {  
  switch (stateMachine.GetState()) {
    case State::Inicio: seguridad(); break;
    case State::Config: menu(); break;
    case State::MonitoreoAmbiental: monitoreoAmbiental(); break;
    case State::MonitoreoEventos: monitoreoEventos(); break;
    case State::Alarma: alarma(); break;
    case State::Bloqueado: bloqueo(); break;
  }
  // Actualizar Maquina de Estados
  stateMachine.Update();
}

// IMPLEMENTACION FUNCIONES
void seguridad() {
  taskTimeoutInicioAFK.Update();

  if (password_attempts == 0) {
    input = BloqueoSistema;
    return;
  }

  const char key = customKeypad.getKey();
  if (!key) return;

  taskTimeoutInicioAFK.Start();

  if (key != 'A') {
    keypadBuffer.push(key);
    lcd.print('*');
    if (!keypadBuffer.isFull()) return;
  }

  if (keypadBuffer.len == password_len && strncmp(password, keypadBuffer.str, password_len) == 0) {
    setLED(0, 1, 0);
    lcd.clear();
    lcd.print("Correcto");
    input = ClaveCorrecta; // <-- Input para la maquina de estados
  } else {
    --password_attempts;
    setLED(1, 1, 0);
    lcd.clear();
    lcd.print("Incorrecto");    
  }
  delay(1000);
  setLED(0, 0, 0);  
  lcd.clear();
  lcd.print("Clave: ");
  keypadBuffer.clear();
}

void bloqueo() {
  setLED(1, 0, 0);
  lcd.setCursor(0, 0);
  lcd.print("Sistema ");
  lcd.setCursor(0, 1);
  lcd.print("Bloqueado");
  execute_melody(melodyBloqueo);
  lcd.clear();
  input = Input::Timeout;
}

void alarma() {
  taskTimeoutAlarma.Update();
  taskTemperatura.Update();
  taskHumedad.Update();
  taskLuz.Update();
  taskHall.Update();
  if (button.check() == LOW) input = Input::BotonPresionado;
}

void menu() {
  const char key = customKeypad.getKey();
  if (button.check() == LOW) input = Input::BotonPresionado;
  if (!key) return;
  switch (mainMenu.get_focusedLine()) {
    case 0: changeVariableControl<float>(&tmp_high, 0.5, key, tmp_low, 100); break;
    case 1: changeVariableControl<float>(&tmp_low, 0.5, key, -50, tmp_high); break;
    case 2: changeVariableControl<int16_t>(&luz_high, 10, key, luz_low, 1000); break;
    case 3: changeVariableControl<int16_t>(&luz_low, 10, key, 0, luz_high); break;
    case 4: changeVariableControl<int16_t>(&hall_high, 10, key, -1000, 1000); break;
    case 5:
      if (key == 'B') {
        tmp_high = DEF_TMP_HIGH;
        tmp_low = DEF_TMP_LOW;
        luz_high = DEF_LUZ_HIGH;
        luz_low = DEF_LUZ_LOW;
        hall_high = DEF_HALL;
      }
      break;
  }
  if (key == 'A') {
    mainMenu.switch_focus();
  }
  mainMenu.update();
}

void monitoreoAmbiental() {
  taskTemperatura.Update();
  taskHumedad.Update();
  taskLuz.Update();
  taskHall.Update();
  taskTimeoutAmbiental.Update();
  taskUpdateMenu.Update();

  if (button.check() == LOW) input = Input::BotonPresionado;
}

void monitoreoEventos() {
  taskTemperatura.Update();
  taskHumedad.Update();
  taskLuz.Update();
  taskHall.Update();
  taskTimeoutEventos.Update();
  taskUpdateMenu.Update();

  if (button.check() == LOW) input = Input::BotonPresionado;
}

template <typename T>
void changeVariableControl(T* variable, T step, char key, T lower_limit, T upper_limit) {
  switch (key) {
    case 'C':
      if (*variable + step > upper_limit) return;
      *variable += step;
      break;
    case 'D':
      if (*variable - step < lower_limit) return;
      *variable -= step;
      break;
  }
}
