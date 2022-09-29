const int led=8;
const int laser=10;
int luce;
void setup() {
  pinMode(led, OUTPUT);
  pinMode(laser, OUTPUT);
  pinMode(A0, OUTPUT);
  digitalWrite(led, LOW);
}

void loop() {
  digitalWrite(laser, 1);
  luce = analogRead(A0);
  if (luce<5){
    tone(3, 500); 
    digitalWrite(led, HIGH);
  }
}
