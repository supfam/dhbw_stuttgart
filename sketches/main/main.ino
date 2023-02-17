/******************** Adafruit Communication Libarys ***************/
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_SPIDevice.h>

/***************************** MQTT *********************************/
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "81.169.194.117";
const char* user = "student";
const char* pass_mqtt = "iotproject2§&d";
const char* clientId = "Team4";


/************* BLYNK ********************************/
#define BLYNK_TEMPLATE_ID           "TMPLUQi3I_My"
#define BLYNK_DEVICE_NAME           "ESP32"
#define BLYNK_AUTH_TOKEN            "2x4QXnlx1UGzWDQY7c4e8bTWzSSRRN9e"
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "iPhone (2)";
char pass[] = "12345abc";
//char ssid[] = "OBI-WLAN KENOBI";
//char pass[] = "b6QUb7G7t6agN66T";

/************* TELEGRAM ********************************/
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#include <WiFiClientSecure.h>

WiFiClientSecure SecureClient;

#define BOTtoken "5354409188:AAG3koVIsRTcKojE2ytDarF2mLCvUmn4rqw"
#define CHAT_ID "-765863793"

UniversalTelegramBot bot(BOTtoken, SecureClient);

/*************STEPPER MOTOR********************************/
#include <Stepper.h>
const int steps_per_rev = 2048;

//define variables
int pos;
int pos1;
int pos2;
int pos3;

#define IN1 12 //DO
#define IN2 14 //D1
#define IN3 33 //D2
#define IN4 27 //D3
Stepper my_Stepper(steps_per_rev, IN1 , IN3, IN2, IN4);

int posreached = 0; //0 if pos not reached / 1 if pos reached
int homePermission = 0;

/************* RFID SENSOR RC522 ********************************/
#include <SPI.h>
#include <MFRC522.h>
#define SS_1_PIN      16
#define SS_2_PIN      4
#define SS_3_PIN      2
#define RST_PIN      17
long chipID;
byte ssPins[] = {SS_1_PIN, SS_2_PIN, SS_3_PIN}; // chip select pins als array initialisieren
#define NR_OF_READERS   3 // Anzahl der RFID Reader
MFRC522 mfrc522[NR_OF_READERS];   // MFRC522-Instanz erstellen // Anzahl der RFID Reader

/*************TEMPSENSOR LM 35DZ ********************************/
#define ADC_TEMP_PIN1 36
#define ADC_TEMP_PIN2 39
#define ADC_TEMP_PIN3 34
#define ADC_TEMP_PIN4 35

/************WATERLEVEL SENSOR********************************/
#define ADC_WATERLEVEL_PIN 32

/************ BOARD EXPANDER MCP23017 I2C  ***************************/
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
Adafruit_MCP23X17 mcp;

/**************** RGB LED ***************************/
#define PIN_RED 13
#define PIN_GREEN 26
#define PIN_RED_2 25
#define PIN_GREEN_2 15
#define PIN_RED_3 5
#define PIN_GREEN_3 0

/************** SONSTIGE VARIABLEN ***************/
int ledNr; // Var for pickbylight system
unsigned long lastMsg1 = 0;
unsigned long lastMsg2 = 0;
unsigned long lastMsg3 = 0;
unsigned long lastMsg4 = 0;
unsigned long lastMsg5 = 0;
unsigned long lastMsg6 = 0;


String werkzeugFach1;
String werkzeugFach2;
String werkzeugFach3;

String currentTool;
String lastTool;
int lastToolVar = 0;

/************ VOID SETUP ***************************/
void setup()
{
  /*********************** PIN SETUP ESP32 ********************************/
  //pinMode(ADC_WATERLEVEL_PIN, INPUT);
  pinMode(22, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  pinMode(SS_1_PIN, OUTPUT);
  pinMode(SS_2_PIN, OUTPUT);
  pinMode(SS_3_PIN, OUTPUT);
  pinMode(RST_PIN, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_RED_2, OUTPUT);
  pinMode(PIN_GREEN_2, OUTPUT);
  pinMode(PIN_RED_3, OUTPUT);
  pinMode(PIN_GREEN_3, OUTPUT);

  /*********************** WIFI SETUP ********************************/
  WiFi.begin(ssid, pass);
  SecureClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  bot.sendMessage(CHAT_ID, "Bot started up", "");

  /*********************** SERIELLER MONITOR SETUP ********************************/
  Serial.begin(115200);

  /*********************** BLYNK ********************************/
  Blynk.begin(auth, ssid, pass);

  /*********************** STEPPER MOTOR ************************/
  my_Stepper.setSpeed(15); // Speed for stepper motor (originally 15)

  /*********************** RFID ********************************/
  SPI.begin();
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Alle RFID-Reader initialisieren & auf S.Monitor ausgeben
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
  }
  // Serial.println("RFID-Chip auflegen, um UID anzuzeigen...");

  /***************** MCP SETUP **********************/
  mcp.begin_I2C(); // I2C verbindung initalisieren
  mcp.pinMode(0, OUTPUT);  // expander pin 0 als output
  mcp.pinMode(1, OUTPUT);
  mcp.pinMode(2, OUTPUT);
  mcp.pinMode(3, OUTPUT);
  mcp.pinMode(4, OUTPUT);
  mcp.pinMode(5, OUTPUT);
  mcp.pinMode(6, OUTPUT);
  mcp.pinMode(7, OUTPUT);
  mcp.pinMode(8, OUTPUT);
  mcp.pinMode(9, OUTPUT);

  /*********** MQTT SETUP ***********/
  client.setServer(mqtt_server, 1883); // client initialisieren mit mqtt server
}

void loop() {
  Blynk.run();
  move_motor();
  read_sensors();
  rfid();
  //ledControl(5);
}

/********************* SENSORDATEN FUNCTION ****************************/
void read_sensors() {

  // Temperatur mit Korrekturfunktion berechnen
  // (siehe https://unsinnsbasis.de/ads1115/#tgt-dia)

  float readTemp1 = analogRead(ADC_TEMP_PIN1);
  float temp1 = (readTemp1 / 1.239 + 100.38) / 10.0;
  Blynk.virtualWrite(V5, temp1);

  if (temp1 > 45) {
    unsigned long  now = millis();
    if (now - lastMsg1 >= 10000) {
      bot.sendMessage(CHAT_ID, "Critical Temperature (Box 1)!", "");
      lastMsg1 = now;
    }
  }

  int red_1 = map(temp1, 20, 100, 0, 150);
  int green_1 = map(temp1, 0, 80, 150, 0);

  analogWrite(PIN_RED, red_1);
  analogWrite(PIN_GREEN, green_1);


  float readTemp2 = analogRead(ADC_TEMP_PIN2);
  float  temp2 = (readTemp2 / 1.239 + 100.38) / 10.0;
  Blynk.virtualWrite(V1, temp2);

  if (temp2 > 45) {
    unsigned long  now = millis();
    if (now - lastMsg2 >= 10000) {
      bot.sendMessage(CHAT_ID, "Critical Temperature (Box 2)!", "");
      lastMsg2 = now;
    }
  }

  int red_2 = map(temp2, 20, 100, 0, 150);
  int green_2 = map(temp2, 0, 80, 150, 0);

  analogWrite(PIN_RED_2, red_2);
  analogWrite(PIN_GREEN_2, green_2);

  float readTemp3 = analogRead(ADC_TEMP_PIN3);
  float temp3 = (readTemp3 / 1.239 + 100.38) / 10.0;
  Blynk.virtualWrite(V8, temp3);

  if (temp3 > 45) {
    unsigned long  now = millis();
    if (now - lastMsg3 >= 10000) {
      bot.sendMessage(CHAT_ID, "Critical Temperature (Werkzeugträger)!", "");
      lastMsg3 = now;
    }
  }


  float readTemp4 = analogRead(ADC_TEMP_PIN4);
  float temp4 = (readTemp4 / 1.239 + 100.38) / 10.0;
  Blynk.virtualWrite(V7, temp4);


  int red_3 = map(temp4, 20, 100, 0, 150);
  int green_3 = map(temp4, 0, 80, 150, 0);

  analogWrite(PIN_RED_3, red_3);
  analogWrite(PIN_GREEN_3, green_3);
  if (temp4 > 45) {
    unsigned long  now = millis();
    if (now - lastMsg4 >= 10000) {
      bot.sendMessage(CHAT_ID, "Critical Temperature (Box 3)!", "");
      lastMsg4 = now;
    }
  }

  float readWaterlevel = analogRead(ADC_WATERLEVEL_PIN);
 // Serial.println(readWaterlevel);
  float waterlevel = map(readWaterlevel, 2800, 4000, 0, 100);
  Blynk.virtualWrite(V6, waterlevel);
 // Serial.println(waterlevel);

  if (waterlevel < 15 && waterlevel > 0 ) {
    unsigned long  now = millis();
    if (now - lastMsg5 >= 10000) {
      bot.sendMessage(CHAT_ID, "Critical Low Waterlevel!", "");
      lastMsg5 = now;
    }
    else if (waterlevel == 0) {
      unsigned long  now = millis();
      if (now - lastMsg5 >= 120000) {
        bot.sendMessage(CHAT_ID, "Waterlevel: 0!", "");
        lastMsg5 = now;
      }
    }
  }
    if (!client.connected()) {   //verbindung mit mqtt sever
      reconnect();
    }
    client.loop();

    //public der daten an dhbw server über mqtt
     unsigned long  now = millis();
    if (now - lastMsg6 > 2000) {
      client.publish("dhbw/team11/value1", String(temp1).c_str());
      client.publish("dhbw/team11/value2", String(temp2).c_str());
      client.publish("dhbw/team11/value3", String(temp3).c_str());
      client.publish("dhbw/team11/value4", String(temp4).c_str());
      client.publish("dhbw/team11/value5", String(waterlevel).c_str());
      lastMsg6 = now;
    }

}

/********************* MOVE-MOTOR FUNCTION ****************************/
void move_motor() {

  if (pos1 == 1) {
    if (posreached == 0) {
      Stepper1(1, 13);     // steppermotor moves to position 1
      //Serial.println("gehe zu pos 1");
      posreached = 1;
      // Serial.println(homePermission);
    }
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
  delay(20);

  if (pos2 == 1) {

    if (posreached == 0) {
      Stepper1(1, 25);
      //Serial.println("go to pos2");
      posreached = 1;
    }
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
  delay(20);

  if (pos3 == 1) {

    if (posreached == 0) {
      Stepper1(1, 37);
      // Serial.println("go to pos3");
      posreached = 1;
    }
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
  delay(20);
}

/********************* RFID FUNCTION ****************************/
void rfid() {

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {  //initialisiert alle drei reader
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);

    if (mfrc522[reader].PICC_IsNewCardPresent()) {  // Sobald ein Chip aufgelegt wird startet diese Abfrage
      chipID = 0;   //CardID resetten
      mfrc522[reader].PICC_ReadCardSerial();

      for (byte i = 0; i < mfrc522[reader].uid.size; i++) {
        chipID = ((chipID + mfrc522[reader].uid.uidByte[i]) * 10);  // Hier wird die ID des Chips in die Variable chipID geladen
      }
      //Serial.println(chipID);

      if (reader == 0) {

        Blynk.virtualWrite(V3, getToolNumberFromUID(chipID));

      }

      if (reader == 1) {

        Blynk.virtualWrite(V0, getToolNumberFromUID(chipID));

      }
      if (reader == 2) {

        Blynk.virtualWrite(V2, getToolNumberFromUID(chipID));
      }
    }


    else if (!mfrc522[reader].PICC_IsNewCardPresent()) {  // wenn kein chip aufliegt

      if (reader == 0) {

        Blynk.virtualWrite(V3, "fach leer");
      }
      if (reader == 1) {

        Blynk.virtualWrite(V0, "fach leer");
      }
      if (reader == 2) {

        Blynk.virtualWrite(V2, "fach leer");
      }

      delay(80);

    }

  }
}

/********** helper function to map the RFID UID to a corresponding Werkzeugnummer (xxxxxx -> "Werkzeug (z)") *******/
String getToolNumberFromUID(int chipID) {

  if (chipID == 1334060) {    //3128575613
    return "Werkzeug 1";
  }

  if (chipID == 773490) {     //0503117874
    return "Werkzeug 2";
  }

  if (chipID == 201880) {     //0470201602
    return "Werkzeug 3";
  }

  if (chipID == 514860) {     //3128439597
    return "Werkzeug 4";
  }

  if (chipID == 1693090) {    //0154330258
    return "Werkzeug 5";
  }

  if (chipID == 1660790) {    //0501289634
    return "Werkzeug 6";
  }

  if (chipID == 705580) {    //0469970242
    return "Werkzeug 7";
  }

  if (chipID == 1006680) {    //0470726994
    return "Werkzeug 8";
  }

  if (chipID == 882490) {   //0502408786
    return "Werkzeug 9";
  }

  if (chipID == 160760) {   //3127710221
    return "Werkzeug 10";
  }

  return ("");
}

// function to map input from Blynk to LED output of MCP23017
void ledControl(int ledNr) {

  if (ledNr == 10) {
    for (int i = 0; i < 10; i++) {
      mcp.digitalWrite(i, LOW);
    }
  }

  for (int i = 0; i < 10; i++) {
    if (i == ledNr) {
      mcp.digitalWrite(i, HIGH);
    }

    else {
      mcp.digitalWrite(i, LOW);
    }
  }
}

/************************STEPPER MOTOR FUNCTION ***********************************/
void Stepper1 (int Direction, int Rotation) {         // function for stepper motor control with 2 parameters
  for (int i = 0; i < Rotation; i++) {                // for loop
    my_Stepper.step(Direction * 200);                  // 200 is 360 degree => change value if smaller then 360 degree is needing
    Blynk.run();
  }
}


/****************** MQTT FUNCTION *********************************/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientId, user, pass_mqtt)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

/****************** communication mit blynk's virtuellen Pins *********************************/
BLYNK_WRITE(V31) {
  homePermission = param.asInt();

  if (homePermission == 1 && posreached == 1) {

    if ( pos1 == 1 ) {
      Blynk.syncVirtual(V3);
      Serial.println("gehe zu home");
      Stepper1(-1, 13); //return to home position
    }

    else if ( pos2 == 1 ) {
      Blynk.syncVirtual(V0);
      Serial.println("gehe zu home");
      Stepper1(-1, 25); //return to home position
    }

    else if ( pos3 == 1 ) {
      Blynk.syncVirtual(V2);
      Serial.println("gehe zu home");
      Stepper1(-1, 37); //return to home position
    }

    lastToolVar = lastToolVar + 1;

    if (lastToolVar == 2) {
      String lastTool = currentTool;
      Blynk.virtualWrite(V33, lastTool);
      lastToolVar = 1;
    }

    posreached = 0;
    pos1 = 0;
    pos2 = 0;
    pos3 = 0;
    homePermission = 0;
    Blynk.virtualWrite(V10, 0);
    Blynk.virtualWrite(V11, 0);
    Blynk.virtualWrite(V12, 0);
    Blynk.virtualWrite(V31, 0);
  }
}

BLYNK_WRITE(V10) {                                     // read input from virtual pin V1
  if (pos2 == 1 or pos3 == 1) {
    pos1 = 0;
    Blynk.virtualWrite(V10, 0);
  }

  else {
    pos1 = param.asInt();                              // assigning incoming value from pin V§1 to a variable
  }
}

BLYNK_WRITE(V11) {
  if (pos1 == 1 or pos3 == 1) {
    pos2 = 0;
    Blynk.virtualWrite(V11, 0);
  }

  else {
    pos2 = param.asInt();                              // assigning incoming value from pin V§1 to a variable
  }
}

BLYNK_WRITE(V12) {
  if (pos1 == 1 or pos2 == 1) {
    pos3 = 0;
    Blynk.virtualWrite(V12, 0);
  }

  else {
    pos3 = param.asInt();                              // assigning incoming value from pin V§1 to a variable
  }
}

BLYNK_WRITE(V20) {

  int ledNr = 0;
  ledControl(ledNr);

}

BLYNK_WRITE(V21) {
  int ledNr = 1;

  ledControl(ledNr);

}
BLYNK_WRITE(V22) {
  int ledNr = 2;
  ledControl(ledNr);

}

BLYNK_WRITE(V23) {
  int ledNr = 3;

  ledControl(ledNr);

}

BLYNK_WRITE(V24) {
  int ledNr = 4;
  ledControl(ledNr);

}

BLYNK_WRITE(V25) {
  int ledNr = 5;
  ledControl(ledNr);

}

BLYNK_WRITE(V26) {
  int ledNr = 6;
  ledControl(ledNr);


}

BLYNK_WRITE(V27) {
  int ledNr = 7;
  ledControl(ledNr);

}

BLYNK_WRITE(V28) {
  int ledNr = 8;
  ledControl(ledNr);

}

BLYNK_WRITE(V29) {
  int ledNr = 9;
  ledControl(ledNr);

}

BLYNK_WRITE(V30) {
  ledControl(10);
}


BLYNK_WRITE(V3) {
  currentTool = param.asString();
  //Serial.print("current Tool: ");
  //Serial.println(currentTool);
  Blynk.virtualWrite(V32, currentTool);
}

BLYNK_WRITE(V0) {
  currentTool = param.asString();
  //Serial.print("current Tool: ");
  //Serial.println(currentTool);
  Blynk.virtualWrite(V32, currentTool);

}

BLYNK_WRITE(V2) {
  currentTool = param.asString();
  //Serial.print("current Tool: ");
  //Serial.println(currentTool);
  Blynk.virtualWrite(V32, currentTool);

}
