 #define LM35_PIN 35

void setup() {
  // Starte die serielle Kommunikation mit einer Baudrate von 9600
  Serial.begin(9600);

}

void loop() {
  // Lese den Wert vom LM35-Sensor aus
  float sensorValue = analogRead(LM35_PIN);
  // Konvertiere den Sensorwert in eine Temperatur in Grad Celsius
  float celsius = (sensorValue);
  // Gib die Temperatur auf dem seriellen Monitor aus
  Serial.print("Temperatur: ");
  Serial.print(celsius);
  Serial.println(" Grad Celsius");
  // Warte eine Sekunde
  delay(1000);
}
