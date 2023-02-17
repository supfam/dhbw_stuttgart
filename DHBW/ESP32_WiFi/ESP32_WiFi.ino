/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP32 chip.

  Note: This requires ESP32 support package:
    https://github.com/espressif/arduino-esp32

  Please be sure to select the right ESP32 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */

#define BLYNK_TEMPLATE_ID "TMPL8Ho6dVcm"
#define BLYNK_DEVICE_NAME  "esp32DHBW"
#define BLYNK_TEMPLATE_NAME "esp32DHBW"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_AUTH_TOKEN "YSb8MO2FRYqe6gsWu-yL6AW3fSxBhvtT"

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "BLYNK_AUTH_TOKEN";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "iPhone (2)";
char pass[] = "12345abc";

void setup()
{
  // Debug console
  Serial.begin(9600);

  // start WiFI
   WiFi.begin(ssid, pass);
    Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("WiFi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();
}
