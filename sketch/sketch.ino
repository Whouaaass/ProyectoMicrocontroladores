/**
  Proyecto Arquitectura Computacional - Microcontroladores
  Hecho por:
  Fredy Esteban Anaya Salazar
  Jorge Andrés ...
  Jhoan David Chacón ...
**/
#include <LiquidMenu.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include "DHT.h"
#include "Button.h"
#include "AsyncTaskLib.h"
#include "StateMachineLib.h"
#include "Average.h"
#include "melody.h"


/* DEFINICION DE PINES */
//#define DHT_TYPE DHT11
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

/* DEFINICION DE PARAMETROS */
#define PASSWORD_ATTEMPTS 3
#define DEF_TMP_HIGH 40
#define DEF_TMP_LOW 10
#define DEF_LUZ_HIGH 650
#define DEF_LUZ_LOW 20
#define DEF_HALL 100

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
char keypadKey = 0;

struct buffer {
  static const int size = 16;
  char str[size + 1];
  byte len = 0;
  void push(char chr) {
    if (len == size) return;
    str[len++] = chr;
  }
  void clear() {
    for (size_t i = 0; i < this->len; i++) {
      str[i] = 0;
    }
    len = 0;
  }
  bool isFull() {
    return len == size;
  }
  char lastCharacter() {
    return len == 0 ? 0 : str[len - 1];
  }
} keypad_input;

/* VARIABLES GLOBALES */
const byte password_len = 4;
const char password[password_len + 1] = "4444";
byte password_attempts;

float tmp_high = DEF_TMP_HIGH;
float tmp_low = DEF_TMP_LOW;
int16_t luz_high = DEF_LUZ_HIGH;
int16_t luz_low = DEF_LUZ_LOW;
int16_t hall_high = DEF_HALL;

float T = 0; /* !< Temperatura */
int16_t H = 0; /* !< Humedad */
int16_t L = 0; /* !< Luz */
int16_t HALL = 0; /* !< Hall */
char* messageAlarma;

Average<float> temperatura(5);
Average<int16_t> humedad(5);
Average<int16_t> luz(5);
Average<int16_t> hall(5);

Button button(BUTTON_PIN, true);

// Funciones para cambiar las variables de control
//void changeLuzHigh() { changeVariableControl<uint16_t>(&luz_high, 1); }
//void changeLuzLow() { changeVariableControl<uint16_t>(&luz_low, 1); }




/* FUNCIONES PREDECLARADAS */

//void readKeypad();
void onEnteringInicio();


void seguridad();
void goodAttempt();
void wrongAttempt();
void menu();
void bloqueo();
void alarma();
void monitoreoAmbiental();
void monitoreoEventos();

void inputTimeout();
void setLED(uint16_t red, uint16_t green, uint16_t blue);
void controlTemperatura();
void controlHumedad();
void controlLuz();
void controlHall();
void blankFunction();




/* TAREAS ASINCRONICAS */
//AsyncTask taskGetKeyPad(100, true, readKeypad);
//AsyncTask taskSeguridad(100, true, seguridad);

AsyncTask taskTemperatura(1000, true, controlTemperatura);
AsyncTask taskHumedad(1000, true, controlHumedad);
AsyncTask taskLuz(1000, true, controlLuz);
AsyncTask taskHall(1000, true, controlHall);
AsyncTask taskTimeoutEventos(3000, false, inputTimeout);
AsyncTask taskTimeoutAmbiental(7000, false, inputTimeout);
AsyncTask taskTimeoutAlarma(4000, false, inputTimeout);
AsyncTask taskTimeoutInicioAFK(7000, false, wrongAttempt);


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

/* CONFIGURACION DE LA MAQUINA DE ESTADOS */
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

StateMachine stateMachine(6, 12);
Input input;
void setupStateMachine() {
  // Inicio
  stateMachine.AddTransition(Inicio, Config, []() { return input == ClaveCorrecta; });
  stateMachine.AddTransition(Inicio, Bloqueado, []() { return input == BloqueoSistema; });

  //Config
  stateMachine.AddTransition(Config, MonitoreoAmbiental, []() { return input == BotonPresionado; });

  //Monitoreo Ambiental
  stateMachine.AddTransition(MonitoreoAmbiental, Config, []() { return input == BotonPresionado; });
  stateMachine.AddTransition(MonitoreoAmbiental, Alarma, []() { return input == Umbral; });
  stateMachine.AddTransition(MonitoreoAmbiental, MonitoreoEventos, []() { return input == Timeout; });

  // Bloqueado
  stateMachine.AddTransition(Bloqueado, Inicio, []() { return input == Timeout; });

  //Monitoreo Eventos
  stateMachine.AddTransition(MonitoreoEventos, Config, []() { return input == BotonPresionado; });
  stateMachine.AddTransition(MonitoreoEventos, MonitoreoAmbiental, []() { return input == Timeout; });
  stateMachine.AddTransition(MonitoreoEventos, Alarma, []() { return input == Umbral; });
  //Alarma
  stateMachine.AddTransition(Alarma, MonitoreoAmbiental, []() { return input == Timeout; });
  stateMachine.AddTransition(Alarma, Inicio, []() { return input == BotonPresionado; });

  stateMachine.SetOnEntering(Inicio, onEnteringInicio);
  stateMachine.SetOnEntering(Config, []() {
    Serial.println("Entering Configuracion");
    input = Input::Unknown;
    mainMenu.change_screen(&scrn_config);
    mainMenu.set_focusedLine(0);
    mainMenu.update();
  });
  stateMachine.SetOnEntering(MonitoreoAmbiental, []() {
    Serial.println("Entering Monitoreo Ambiental");
    input = Input::Unknown;
    taskTemperatura.Start();
    taskHumedad.Start();
    taskLuz.Start();
    taskHall.Start();
    taskTimeoutAmbiental.Start();
    mainMenu.change_screen(&scrn_monitoreo_ambiental);
    mainMenu.update();
  });
  stateMachine.SetOnEntering(Bloqueado, []() {
    Serial.println("Entering Bloqueado");
    input = Input::Unknown;

    bloqueo();
  });
  stateMachine.SetOnEntering(MonitoreoEventos, []() {
    Serial.println("Entering Monitoreo Eventos");
    input = Input::Unknown;
    taskTemperatura.Start();
    taskHumedad.Start();
    taskLuz.Start();
    taskHall.Start();
    taskTimeoutEventos.Start();
    mainMenu.change_screen(&scrn_monitoreo_eventos);
    mainMenu.update();
  });
  stateMachine.SetOnEntering(Alarma, []() {
    Serial.println("Entering Alarma");
    input = Input::Unknown;
    setLED(0, 0, 1);
    tone(buzzer, 97, 4000);
    taskTemperatura.Start();
    taskHumedad.Start();
    taskLuz.Start();
    taskHall.Start();
    taskTimeoutAlarma.Start();
    mainMenu.change_screen(&scrn_alarma);
    mainMenu.update();
  });
  stateMachine.SetOnLeaving(Inicio, []() {
    Serial.println("Leaving Inicio");
    taskTimeoutInicioAFK.Stop();
  });
  stateMachine.SetOnLeaving(Config, []() {
    Serial.println("Leaving Configuracion");
  });
  stateMachine.SetOnLeaving(MonitoreoAmbiental, []() {
    Serial.println("Leaving Monitoreo Ambiental");
    taskTemperatura.Stop();
    taskHumedad.Stop();
    taskLuz.Stop();
    taskHall.Stop();
    taskTimeoutAmbiental.Stop();
  });
  stateMachine.SetOnLeaving(Bloqueado, []() {
    Serial.println("Leaving Bloqueado");
    setLED(0, 0, 0);
  });
  stateMachine.SetOnLeaving(MonitoreoEventos, []() {
    Serial.println("Leaving Monitoreo Eventos");
    taskTemperatura.Stop();
    taskHumedad.Stop();
    taskLuz.Stop();
    taskHall.Stop();
    taskTimeoutEventos.Stop();
  });
  stateMachine.SetOnLeaving(Alarma, []() {
    Serial.println("Leaving Alarma");
    taskTemperatura.Stop();
    taskHumedad.Stop();
    taskLuz.Stop();
    taskHall.Stop();
    taskTimeoutAlarma.Stop();
    setLED(0, 0, 0);
  });
}

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
  }

  // Actualizar Maquina de Estados
  stateMachine.Update();
}

/* Implementacion funciones */

void inputTimeout() {
  input = Input::Timeout;
}

void setLED(uint16_t red, uint16_t green, uint16_t blue) {
  digitalWrite(LED_R, red);
  digitalWrite(LED_G, green);
  digitalWrite(LED_B, blue);
}

bool readKeypad() {
  char customKey = customKeypad.getKey();
  if (!customKey) return false;
  keypad_input.push(customKey);
  return true;
}
void controlTemperatura() {
  T = temperatura.rolling(dht.readTemperature());
  mainMenu.update();
  if (T > tmp_high) {
    messageAlarma = "Temperatura";
    input = Input::Umbral;
    }
}
void controlHumedad() {
  H = humedad.rolling(dht.readHumidity());
  mainMenu.update();
}
void controlLuz() {
  L = luz.rolling(analogRead(PHOTOCELL_PIN));
  mainMenu.update();
  if (L > luz_high) {
    messageAlarma = "Luz";
    input = Input::Umbral;
    }
}
void controlHall() {
  HALL = hall.rolling(analogRead(HALL_PIN));
  mainMenu.update();
  if (HALL > hall_high) {
    messageAlarma = "Hall";
    input = Input::Umbral;
    }
}

void seguridad() {
  // Si la contrasenia se ingreso muchas veces
  taskTimeoutInicioAFK.Update();
  if (password_attempts == 0) {
    input = Input::BloqueoSistema;
    return;
  }

  bool readed = readKeypad();
  if (readed) {
    lcd.print('*');
    taskTimeoutInicioAFK.Start();
  }

  // Si aun no se completa el ingreso de la contrasenia
  if (!keypad_input.isFull() && keypad_input.lastCharacter() != 'A') {
    return;
  }
  
  if (keypad_input.len - 1 == password_len && strncmp(password, keypad_input.str, password_len) == 0) {
    goodAttempt();
  } else {
    wrongAttempt();
  }  
}

void wrongAttempt() {
 setLED(1, 1, 0);
 lcd.clear();
  lcd.print("Incorrecto");
  delay(1000);
  setLED(0, 0, 0);
  --password_attempts;
  lcd.clear();
  lcd.print("Clave: ");
  keypad_input.clear();
}
void goodAttempt() {
  setLED(0, 1, 0);
  lcd.clear();
  lcd.print("Correcto");
  delay(1000);
  setLED(0, 0, 0);
  input = Input::ClaveCorrecta;
  keypad_input.clear();
  lcd.print("Clave:");
  keypad_input.clear();
}

void bloqueo() {
  setLED(1, 0, 0);
  lcd.setCursor(0, 0);
  lcd.print("Sistema ");
  lcd.setCursor(0, 1);
  lcd.print("Bloqueado");
  melody_execute();
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

  if (button.check() == LOW) input = Input::BotonPresionado;
}

void monitoreoEventos() {
  taskTemperatura.Update();
  taskHumedad.Update();
  taskLuz.Update();
  taskHall.Update();
  taskTimeoutEventos.Update();

  if (button.check() == LOW) input = Input::BotonPresionado;
}

void blankFunction() {
  Serial.println("Hay un cambio");
}

template <typename T>
void changeVariableControl(T* variable, T step, char key, T lower_limit, T upper_limit) {
  switch (key) {
    case 'C':
      if (*variable + step > upper_limit) return;
      *variable += step; mainMenu.update();
      break;
    case 'D':
      if (*variable - step < lower_limit) return;
      *variable -= step; mainMenu.update();
      break;
  }
}

void onEnteringInicio() {
  Serial.println("Entering Inicio");
  input = Input::Unknown;
  keypad_input.clear();    
  password_attempts = PASSWORD_ATTEMPTS;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Clave: ");
}

