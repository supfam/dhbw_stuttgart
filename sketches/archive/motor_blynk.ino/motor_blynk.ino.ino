
#define BLYNK_TEMPLATE_ID           "TMPLruDBr_hD"
#define BLYNK_DEVICE_NAME           "NodeMCU"
#define BLYNK_AUTH_TOKEN            "dxB_fDM1J2RPB-mgQeZgM4y3qyv8Ww7G"
// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

//define variables
int pos;
int pos1;
int pos2;
int pos3;

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "iPhone";
char pass[] = "12345abc";

//Step Motor

#include <Stepper.h>

const int steps_per_rev = 2048;
#define IN1 D0
#define IN2 D1
#define IN3 D2
#define IN4 D3

Stepper my_Stepper(steps_per_rev, D0, D2, D1, D3);


//Temp sensor LM 35DZ
#define LM35_PIN A0  // GPIO-Pin für die Datenleitung des LM35DZ
int LM35_Value  ;   // Messwert des Sensors
double temperature;  // Temperatur in Grad Celsius


void setup()
{
  my_Stepper.setSpeed(15);                            // Speed for stepper motor

  //LED PIN
  pinMode(D4, OUTPUT);


  // Debug console
  Serial.begin(115200);

  // Bynk
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

BLYNK_WRITE(V10){                                      // read input from virtual pin V1
  pos1 = param.asInt();                              // assigning incoming value from pin V§1 to a variable
  }
  
BLYNK_WRITE(V11){                                      // read input from virtual pin V1
  pos2 = param.asInt();                              // assigning incoming value from pin V§1 to a variable
  }
  
BLYNK_WRITE(V12){                                      // read input from virtual pin V1
  pos3 = param.asInt();                              // assigning incoming value from pin V§1 to a variable
  }


void Stepper1 (int Direction, int Rotation){          // function for stepper motor control with 2 parameters
  for (int i = 0; i < Rotation; i++){                 // for loop 
  my_Stepper.step(Direction * 200);                   // 200 is 360 degree => change value if smaller then 360 degree is needing
  Blynk.run();
  }
}

void loop()
{

  Blynk.run();

  if (pos1 == 1){                                     // if condition 
  Stepper1(1, 5);                                     // steppermotor rotates 10 times 360 degree right
  Serial.println("turn 1/3 of the way");
  delay(500);
  Stepper1(-1,5);
  //Blynk.virtualWrite(V10, 0);
  //pos = 0;
  }
  delay(20);                                           // delay 20ms
  
  if (pos2 == 1){                                           // if condition 
  Stepper1(1, 10);                                    // steppermotor rotates 10 times 360 degree left
  Serial.println("turn 2/3 of the way");
  delay(500);
  Stepper1(-1, 10);
  //Blynk.virtualWrite(V11, 0);  
  //pos = 0;
  }
  delay(20);   

  if (pos3 == 1){                                           // if condition 
  Stepper1(1, 15);                                    // steppermotor rotates 10 times 360 degree left
  Serial.println("turn all the way");
  delay(500);
  Stepper1(-1, 15);  
  //Blynk.virtualWrite(V12, 0);  
  //pos = 0;
  }
  delay(20); 

  LM35_Value = analogRead(LM35_PIN);
  //Serial.print(LM35_Value);
  //Serial.print(" - ");
  // Wert in Temperatur umrechnen
  temperature = (float)LM35_Value * 500 / 1023;
  //Serial.print(temperature, 1);  // eine Nachkommastelle
  //Serial.println(" °C");
  Blynk.virtualWrite(V1, temperature);
  // Serial.println("Pos: "+ motor.currentPosition() );

}


// turn leds on and off

BLYNK_WRITE(V0) {
  int pinValue = param.asInt();

  if (pinValue == 1) {
    digitalWrite(D4, HIGH); // Turn LED on.
  }

  else {
    digitalWrite(D4, LOW);; // Turn LED off.
  }


}
