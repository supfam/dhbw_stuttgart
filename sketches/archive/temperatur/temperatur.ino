int sensorPin = A0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead(sensorPin);
  int temp = map(val, 0, 307, 0, 100);
  Serial.print("temp: ");
  Serial.println(temp);
  delay(500);
}
