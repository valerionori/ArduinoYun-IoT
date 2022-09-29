#define buzzer1 10
#define buzzer2 6

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer1, OUTPUT);
  pinMode(buzzer2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  tone(buzzer2, 1000);
  delay(100);
  noTone(buzzer2);
  delay(100);
}
