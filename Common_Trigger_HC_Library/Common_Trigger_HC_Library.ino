// ------------------------------------------ //
//
//  Univeristy of Modena and Reggio Emilia
//
//          Electronics Engineering
//   Networking Technologies and Protocols
//
//              VALERIO NORI
//
//             28 - 11 - 2021 PROVA
//
// ------------------------------------------ //

#include <HCSR04.h>

#define G_LED0     3      // Silos' 0 Green LED
#define Y_LED0     2      // Silos' 0 Yellow LED and respective Storage's Yellow LED
#define R_LEDB0    12      // Silos' 0 Red LED and respective Buzzer
#define LASER0     13     // Storage's 0 LASER (KY-008 Module)

#define PRX_TLP    6      // Silos' 0 and Silos' 1 Proximity Sensor Triggers. Silos' 0 and Silos' 1 Level Sensor Triggers (HC-SR04 Module)

#define PRX_EL0    7      // Silos' 0 Level Sensor Echo (HC-SR04 Module)
#define PRX_EP0    8      // Silos' 0 Proximity Sensor Echo (HC-SR04 Module)
#define PRX_EL1    9      // Silos' 0 Level Sensor Echo (HC-SR04 Module)
#define PRX_EP1    10     // Silos' 0 Proximity Sensor Echo (HC-SR04 Module)


byte triggerPin = 6;
byte echoCount = 4;
byte* echoPins = new byte[echoCount] { 7, 8, 9, 10 };


void setup() {
  
  // Digital pin setup
  pinMode(G_LED0, OUTPUT);
  pinMode(Y_LED0, OUTPUT);
  pinMode(R_LEDB0, OUTPUT);
  pinMode(LASER0, OUTPUT);
  
  pinMode(PRX_TLP, OUTPUT);
  pinMode(PRX_EL0, INPUT);
  pinMode(PRX_EP0, INPUT);
  pinMode(PRX_EL1, INPUT);
  pinMode(PRX_EP1, INPUT);

  // Analog pin setup
  pinMode(A1, INPUT);        // Photoresistor 0 response (Silo's 0 Laser System)

  Serial.begin(9600);

  // Activate Laser Level Sensors
  digitalWrite(LASER0, HIGH);

  HCSR04.begin(triggerPin, echoPins, echoCount);
}

void loop () {
  double* distances = HCSR04.measureDistanceCm();

  for (int i = 0; i < echoCount; i++) {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(distances[i]);
    Serial.println(" cm");
  }

  /// LEVEL ///
  if(distances[0] < 9 && !digitalRead(G_LED0)){
    digitalWrite(Y_LED0, HIGH);
  }else {
    digitalWrite(Y_LED0, LOW);
  }

  if(distances[0] > 9){
    digitalWrite(G_LED0, LOW);
  }

  /// LASER ///
  if (analogRead(A1)<200){                         
    digitalWrite(Y_LED0, LOW);
    digitalWrite(G_LED0, HIGH);
  }

  /// PROXIMITY ///
  if(distances[1] < 30 && digitalRead(G_LED0)){
    tone(R_LEDB0, 1000);
  }else noTone(R_LEDB0);

  Serial.println(analogRead(A1));
  
  Serial.println("---");
  delay(250);
}
