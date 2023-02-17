// Definiere die Nummer des Pins, an dem die LED angeschlossen ist
const int LED_PIN = 12;

void setup() {
  // Konfiguriere den Pin als Ausgang
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // Schalte die LED ein
  digitalWrite(LED_PIN, HIGH);
  // Warte eine Sekunde
  delay(1000);
  // Schalte die LED aus
  digitalWrite(LED_PIN, LOW);
  // Warte eine weitere Sekunde
  delay(1000);
}
