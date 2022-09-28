/* 
 *            Univeristy of Modena and Reggio Emilia
 *
 *                 "Enzo Ferrari" Eng. Dept.
 *
 *                 Electronics Engineering
 *            Networking Technologies and Protocols
 *
 *                        IoT Project
 *
 *
 *                       VALERIO NORI
 *
 *
 *                      01 - 02 - 2022
 *                      
 *                            v2
 *
*/

/*
 *  Main changes of v2 (Arduino Sketch and HTML/JavaScript):
 *  
 *  - Now you can freely assign each Silos to a specific Deposit.
 *  - A new button for accessing the whole measurements history has been introduced.
 *  - Silos' Measurements Monitor on the Web Page now updates automatically. REFRESH buttons have been removed.
 *  - Overall code performance has been much improved.
 */

#include <HCSR04.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <Process.h>
#include <FileIO.h>

#define Y_LED_A    6      // Deposit A Yellow LED
#define Y_LED_B    13     // Deposit B Yellow LED
#define G_LED0     2      // Silos' 1 Green LED
#define G_LED1     3      // Silos' 2 Green LED
#define Y_LED0     4      // Silos' 1 Yellow LED
#define Y_LED1     5      // Silos' 2 Yellow LED
#define R_LEDB     7      // Red LED and respective Buzzer
#define PRX_T01    8      // Silos' 1 and Silos' 2 Proximity Sensor Triggers. Silos' 1 and Silos' 2 Level Sensor Triggers (HC-SR04 Module)
#define PRX_EL0    9      // Silos' 1 Level Sensor Echo (HC-SR04 Module)
#define PRX_EL1    10     // Silos' 2 Level Sensor Echo (HC-SR04 Module)
#define PRX_EP0    11     // Silos' 1 Proximity Sensor Echo (HC-SR04 Module)
#define PRX_EP1    12     // Silos' 2 Proximity Sensor Echo (HC-SR04 Module)

// Server
BridgeServer server;

//  HCSR04 library parameters
byte triggerPin = 8;                                            //  Common Trigger PIN
byte echoCount = 4;                                             //  Number of Echoes 
byte* echoPins = new byte[echoCount] {9, 10, 11, 12 };          //  Echo PINs

/*
 * PIN 9  -> Silos 1 Sensor Level
 * PIN 10 -> Silos 2 Sensor Level
 * PIN 11 -> Silos 1 Proximity Sensor
 * PIN 12 -> Silos 2 Proximity Sensor
 */

//  Time Strings
String dataString;
String dataStringL1min;
String dataStringL2min;

//  Silos Level Thresholds Configuration
float L1Min;
float L2Min;

//  Email Configuration
int calls1 = 0;
int calls2 = 0;
int maxCalls = 1;
int email_enabled;

//  level.txt size limit.
//  This is done in order to avoid that the levelprox.txt file grows indefinitely.
int writes =  0;

//  Indicates which deposit is assigned to each silos
int pair;

/*
 * pair = 11 : silos 1 with deposit A; silos 2 with deposit B
 * pair = 12 : silos 1 with deposit B; silos 2 with deposit A
 * pair = 10 : silos 1 and silos 2 with deposit A
 * pair = 01 : silos 1 and silos 2 with deposit B
 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                   void setup()                                           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(9600);
  while(!Serial);
  Serial.println("Configuration...");
  
  // Digital pin setup
  pinMode(Y_LED_A, OUTPUT);
  pinMode(Y_LED_B, OUTPUT);
  pinMode(G_LED0, OUTPUT);
  pinMode(G_LED1, OUTPUT);
  pinMode(Y_LED0, OUTPUT);
  pinMode(Y_LED1, OUTPUT);
  pinMode(R_LEDB, OUTPUT);
  pinMode(PRX_T01, OUTPUT);
  pinMode(PRX_EL0, INPUT);
  pinMode(PRX_EL1, INPUT);  
  pinMode(PRX_EP0, INPUT);
  pinMode(PRX_EP1, INPUT);
  Serial.println("Digital PINs Setup Complete");

  // Analog pin setup
  pinMode(A0, INPUT);        // Photoresistor 0 response (Deposit A Laser System)
  pinMode(A1, INPUT);        // Photoresistor 1 response (Deposit B Laser System)
  Serial.println("Analog PINs Setup Complete");

  // PIN Functioning Check
  digitalWrite(Y_LED_A, HIGH);
  digitalWrite(Y_LED_B, HIGH);
  digitalWrite(Y_LED0, HIGH);
  digitalWrite(Y_LED1, HIGH);
  digitalWrite(G_LED0, HIGH);
  digitalWrite(G_LED1, HIGH);
  digitalWrite(R_LEDB, HIGH);
  delay(2000);
  if(digitalRead(Y_LED_A) && digitalRead(Y_LED_B) && digitalRead(Y_LED0) && digitalRead(Y_LED1) && digitalRead(G_LED0) && digitalRead(G_LED1) && digitalRead(R_LEDB)){
    digitalWrite(Y_LED_A, LOW);
    digitalWrite(Y_LED_B, LOW);
    digitalWrite(Y_LED0, LOW);
    digitalWrite(Y_LED1, LOW);
    digitalWrite(G_LED0, LOW);
    digitalWrite(G_LED1, LOW);
    digitalWrite(R_LEDB, LOW);
    delay(2000);
    if(!digitalRead(Y_LED_A) && !digitalRead(Y_LED_B) && !digitalRead(Y_LED0) && !digitalRead(Y_LED1) && !digitalRead(G_LED0) && !digitalRead(G_LED1) && !digitalRead(R_LEDB)){
      Serial.println("PIN Check Passed");
    }else Serial.println("PIN Check Failed");
  }else Serial.println("PIN Check Failed");

  // Control 
  HCSR04.begin(triggerPin, echoPins, echoCount);
  Serial.println("HC-SR04 Setup Complete");
  
  Bridge.begin();
  Serial.println("Bridge Setup Complete");
  
  FileSystem.begin();

  //  Reset the measurements history
  FileSystem.remove("/mnt/sda1/www/level.txt");

  //  Set the silos level threshold values
  File confFileThresh = FileSystem.open("/mnt/sda1/www/limit.txt", FILE_READ);
  L1Min = confFileThresh.parseFloat();
  L2Min = confFileThresh.parseFloat();
  confFileThresh.close();

  //  Set the Email Notification
  File confFileEmail = FileSystem.open("/mnt/sda1/www/email.txt", FILE_READ);
  email_enabled = confFileEmail.parseInt();
  confFileEmail.close();

  //  Set Silos/Deposit Configuration
  File confFilePair = FileSystem.open("/mnt/sda1/www/pair.txt", FILE_READ);
  pair = confFilePair.parseInt();
  confFilePair.close();
  Serial.println("FileSystem Setup Complete");

  // Listen for incoming connection only from localhost
  server.listenOnLocalhost();
  server.begin();

  Serial.println("Setup Complete");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                 void loop()                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  //  Get clients coming from server
  BridgeClient client = server.accept();

  //  There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }

  delay(50);

  //  Retrieve date and time 
  String dataString;
  dataString += getTimeStamp();

  //  Compute the distances of every HC-SR04 sensor
  double* distances = HCSR04.measureDistanceCm();

  //  Print the computed distances on the Serial Monitor
  Serial.println(dataString);
  for (int i = 0; i < echoCount; i++) {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(distances[i]);
    Serial.println(" cm");
  }

  //  Convert measurements into strings
  String L0 = String(distances[0]);
  String L1 = String(distances[1]);

  File dataFile = FileSystem.open("/mnt/sda1/www/level.txt", FILE_APPEND);
  
  //  Write the measurements on a file
  if (dataFile && writes < 100) {
    dataFile.println(dataString);
    dataFile.print("Silos 1 Level = ");
    dataFile.print(L0);
    dataFile.println(" cm ");
    dataFile.print("Silos 2 Level= ");
    dataFile.print(L1);
    dataFile.println(" cm");
    dataFile.close();
    writes++;
  }else{
    FileSystem.remove("/mnt/sda1/www/level.txt");
    FileSystem.open("/mnt/sda1/www/level.txt", FILE_APPEND);
    writes = 0;
  }
  
  // Print the analog value read from the voltage divider (in order to properly tune the Laser System Thresholds)
  Serial.println(analogRead(A0));
  Serial.println(analogRead(A1));

  //  Execute the specific function for the associated Silos/Deposit configuration

  if(pair == 11){
    pair11();
  }

  if(pair == 12){
    pair12();
  }

  if(pair == 10){
    pair10();
  }

  if(pair == 01){
    pair01();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                 FUNCTIONS                                                //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//          Silos 1 paired with Deposit A; Silos 2 paired with Deposit B (pair == 11)            //
///////////////////////////////////////////////////////////////////////////////////////////////////

void pair11 (){

  double* distances = HCSR04.measureDistanceCm();

  ////////////////////////////////// LEVEL SENSOR SYSTEM //////////////////////////////////
  /*  
   *   If a silos is empty, the Yellow LEDs on it and on the respective deposit will light up.
   *   Moreover, if a silos is full again, its Green LED will turn off
   */
  
  //  Silos 1
  if(distances[0] > L1Min && !digitalRead(G_LED0)){
    digitalWrite(Y_LED0, HIGH);
    digitalWrite(Y_LED_A, HIGH);
  }else digitalWrite(Y_LED0, LOW);

  if(distances[0] < L1Min){
    digitalWrite(G_LED0, LOW);
    digitalWrite(Y_LED_A, LOW);
    calls1 = 0;
  }

  //  If silos 1 becomes too empty, the technological laboratory is warned through an email
  if(distances[0] > (L1Min + 2) && (digitalRead(Y_LED0) || digitalRead(G_LED0)) && calls1 < maxCalls && email_enabled == 1){
    runSendEmail(0x00);
    calls1 ++;
  }

  //  Silos 2
  if(distances[1] > L1Min && !digitalRead(G_LED1)){
    digitalWrite(Y_LED1, HIGH);
    digitalWrite(Y_LED_B, HIGH);
  }else digitalWrite(Y_LED1, LOW);

  if(distances[1] < L2Min){
    digitalWrite(G_LED1, LOW);
    digitalWrite(Y_LED_B, LOW);
    calls2 = 0;
  }

  //  If silos 2 becomes too empty, the technological laboratory is warned through an email
  if(distances[1] > (L2Min + 2) && (digitalRead(Y_LED1) || digitalRead(G_LED1)) && calls2 < maxCalls && email_enabled == 1){
    runSendEmail(0x01);
    calls2 ++;
  }

  ////////////////////////////////// LASER SYSTEM //////////////////////////////////
  /*   
   *   If no light arrives to the photoresistor, it means that the excavator is taking the
   *   raw materials from that storage, so the Green LED on its respective silos will light up.
   *   (Yellow LED == HIGH) is an additional condition in order to avoid to light up the Green
   *   LED even if the silos is not empty and someone activate the laser system.
   */

  // Deposit 1
  if(analogRead(A0)>700 && digitalRead(Y_LED0)){
    digitalWrite(G_LED0, HIGH);
  }
  
  //  Deposit 2
  if(analogRead(A1)>700 && digitalRead(Y_LED1)){
    digitalWrite(G_LED1, HIGH);
  }

  ////////////////////////////////// ALARM SYSTEM //////////////////////////////////
  /*
   *   If the excavator goes near the wrong silos (e.g., it goes near silos 0 instead of silos 1),
   *   the Red LED of the wrong silos will light up and its buzzer will activate.
   */

  //  Silos 1
  if((distances[2] < 8 && digitalRead(G_LED1)) || (distances[3] < 8 && digitalRead(G_LED0))){
    tone(R_LEDB, 1000);
  }else noTone(R_LEDB);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//          Silos 1 paired with Deposit B; Silos 2 paired with Deposit A (pair == 12)            //
///////////////////////////////////////////////////////////////////////////////////////////////////

void pair12 (){

  double* distances = HCSR04.measureDistanceCm();

  ////////////////////////////////// LEVEL SENSOR SYSTEM //////////////////////////////////
    
  //  Silos 1
  if(distances[0] > L1Min && !digitalRead(G_LED0)){
    digitalWrite(Y_LED0, HIGH);
    digitalWrite(Y_LED_B, HIGH);
  }else{
    digitalWrite(Y_LED0, LOW);
    digitalWrite(Y_LED_B, LOW);
  }

  if(distances[0] < L1Min){
    digitalWrite(G_LED0, LOW);
    calls1 = 0;
  }

  //  If silos 1 becomes too empty, the technological laboratory is warned through an email
  if(distances[0] > (L1Min + 2) && (digitalRead(Y_LED0) || digitalRead(G_LED0)) && calls1 < maxCalls && email_enabled == 1){
    runSendEmail(0x00);
    calls1 ++;
  }

  //  Silos 2
  if(distances[1] > L2Min && !digitalRead(G_LED1)){
    digitalWrite(Y_LED1, HIGH);
    digitalWrite(Y_LED_A, HIGH);
  }else{
    digitalWrite(Y_LED1, LOW);
    digitalWrite(Y_LED_A, LOW);
  }

  if(distances[1] < L2Min){
    digitalWrite(G_LED1, LOW);
    calls2 = 0;
  }

  //  If silos 2 becomes too empty, the technological laboratory is warned through an email
  if(distances[1] > (L2Min + 2) && (digitalRead(Y_LED1) || digitalRead(G_LED1)) && calls2 < maxCalls && email_enabled == 1){
    runSendEmail(0x01);
    calls2 ++;
  }

  ////////////////////////////////// LASER SYSTEM //////////////////////////////////

  // Deposit 1
  if(analogRead(A1)>700 && digitalRead(Y_LED0)){
    digitalWrite(G_LED0, HIGH);
  }
  
  //  Deposit 2
  if(analogRead(A0)>700 && digitalRead(Y_LED1)){
    digitalWrite(G_LED1, HIGH);
  }

  ////////////////////////////////// ALARM SYSTEM ////////////////////////////////// 

  //  Silos 1
  if((distances[2] < 8 && digitalRead(G_LED1)) || (distances[3] < 8 && digitalRead(G_LED0))){
    tone(R_LEDB, 1000);
  }else noTone(R_LEDB);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                  Silos 1 and Silos 2 paired with Deposit A (pair == 10)                       //
///////////////////////////////////////////////////////////////////////////////////////////////////

void pair10 (){

  double* distances = HCSR04.measureDistanceCm();

  ////////////////////////////////// LEVEL SENSOR SYSTEM //////////////////////////////////
    
  //  Silos 1
  if(distances[0] > L1Min && !digitalRead(G_LED0)){
    digitalWrite(Y_LED0, HIGH);
    digitalWrite(Y_LED_A, HIGH);
  }else{
    digitalWrite(Y_LED0, LOW);
  }

  if(distances[0] < L1Min){
    digitalWrite(G_LED0, LOW);
    calls1 = 0;
  }

  //  If silos 1 becomes too empty, the technological laboratory is warned through an email
  if(distances[0] > (L1Min + 2) && (digitalRead(Y_LED0) || digitalRead(G_LED0)) && calls1 < maxCalls && email_enabled == 1){
    runSendEmail(0x00);
    calls1 ++;
  }

  //  Silos 2
  if(distances[1] > L2Min && !digitalRead(G_LED1)){
    digitalWrite(Y_LED1, HIGH);
    digitalWrite(Y_LED_A, HIGH);
  }else{
    digitalWrite(Y_LED1, LOW);
  }

  if(distances[1] < L2Min){
    digitalWrite(G_LED1, LOW);
    calls2 = 0;
  }

  //  If silos 2 becomes too empty, the technological laboratory is warned through an email
  if(distances[1] > (L2Min + 2) && (digitalRead(Y_LED1) || digitalRead(G_LED1)) && calls2 < maxCalls && email_enabled == 1){
    runSendEmail(0x01);
    calls2 ++;
  }

  if(distances[0] < L1Min && distances[1] < L2Min){
    digitalWrite(Y_LED_A, LOW);
  }

  ////////////////////////////////// LASER SYSTEM //////////////////////////////////

  // Deposit 1
  if(analogRead(A0)>700 && digitalRead(Y_LED0) && digitalRead(Y_LED1)){
    digitalWrite(G_LED0, HIGH);
    digitalWrite(G_LED1, HIGH);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                  Silos 1 and Silos 2 paired with Deposit B (pair == 01)                       //
///////////////////////////////////////////////////////////////////////////////////////////////////

void pair01 (){

  double* distances = HCSR04.measureDistanceCm();

  ////////////////////////////////// LEVEL SENSOR SYSTEM //////////////////////////////////
    
  //  Silos 1
  if(distances[0] > L1Min && !digitalRead(G_LED0)){
    digitalWrite(Y_LED0, HIGH);
    digitalWrite(Y_LED_B, HIGH);
  }else{
    digitalWrite(Y_LED0, LOW);
  }

  if(distances[0] < L1Min){
    digitalWrite(G_LED0, LOW);
    calls1 = 0;
  }

  //  If silos 1 becomes too empty, the technological laboratory is warned through an email
  if(distances[0] > (L1Min + 2) && (digitalRead(Y_LED0) || digitalRead(G_LED0)) && calls1 < maxCalls && email_enabled == 1){
    runSendEmail(0x00);
    calls1 ++;
  }

  //  Silos 2
  if(distances[1] > L2Min && !digitalRead(G_LED1)){
    digitalWrite(Y_LED1, HIGH);
    digitalWrite(Y_LED_B, HIGH);
  }else{
    digitalWrite(Y_LED1, LOW);
  }

  if(distances[1] < L2Min){
    digitalWrite(G_LED1, LOW);
    calls2 = 0;
  }

  //  If silos 2 becomes too empty, the technological laboratory is warned through an email
  if(distances[1] > (L2Min + 2) && (digitalRead(Y_LED1) || digitalRead(G_LED1)) && calls2 < maxCalls && email_enabled == 1){
    runSendEmail(0x01);
    calls2 ++;
  }

  if(distances[0] < L1Min && distances[1] < L2Min){
    digitalWrite(Y_LED_B, LOW);
  }

  ////////////////////////////////// LASER SYSTEM //////////////////////////////////

  // Deposit 1
  if(analogRead(A1)>700 && digitalRead(Y_LED0) && digitalRead(Y_LED1)){
    digitalWrite(G_LED0, HIGH);
    digitalWrite(G_LED1, HIGH);
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                     Client Process                                            //
///////////////////////////////////////////////////////////////////////////////////////////////////

void process(BridgeClient client) {
  // read the command
  String command = client.readStringUntil('/');

  // is "param" command?
  if (command == "param"){
    paramCommand(client);
    }
  if (command == "conf"){
    confCommand(client);
    }    
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                    Parameter Commands                                         //
///////////////////////////////////////////////////////////////////////////////////////////////////

void paramCommand(BridgeClient client){
  
  double* distances = HCSR04.measureDistanceCm(); 
  String dataString;
  dataString += getTimeStamp();
  
  String parameter = client.readStringUntil('\r'); // continues scanning the URL

  //  Display both silos level
  if (parameter == "all"){
   client.println(distances[0]);
   client.println(distances[1]);
  }

  //  Display silos 1 level only
  if (parameter == "l1"){
   client.println(distances[0]);
  }

  //  Display silos 2 level only
  if (parameter == "l2"){
   client.println(distances[1]);
  }

  //  Reset measurements history
  if (parameter == "resetHistory"){
    FileSystem.remove("/mnt/sda1/www/level.txt");
    File dataFileReset = FileSystem.open("/mnt/sda1/www/level.txt", FILE_APPEND);  //re-create the file
    dataFileReset.close(); 
    client.println(F("History has been successfully reset"));
    }

   // Reset Limit Thresholds
    if (parameter == "resetStat"){
      L1Min = 8;
      L2Min = 8;
      client.println(F("All Thresholds have been successfully reset"));
    }

  //  Change Silos - Deposit pairs
  if (parameter == "11" && !digitalRead(Y_LED_A) && !digitalRead(Y_LED_B)){
    pair = 11;
    FileSystem.remove("/mnt/sda1/www/pair.txt");
    File pairFileReset = FileSystem.open("/mnt/sda1/www/pair.txt", FILE_WRITE);  //re-create the file
    pairFileReset.print(pair);
    pairFileReset.close(); 
    client.println(F("Setup has been successfully updated"));
  }

  if (parameter == "12" && !digitalRead(Y_LED_A) && !digitalRead(Y_LED_B)){
    pair = 12;
    File pairFileReset = FileSystem.open("/mnt/sda1/www/pair.txt", FILE_WRITE);  //re-create the file
    pairFileReset.print(pair);
    pairFileReset.close(); 
    client.println(F("Setup has been successfully updated"));
  }
  
  if (parameter == "10" && !digitalRead(Y_LED_A) && !digitalRead(Y_LED_B)){
    pair = 10;
    File pairFileReset = FileSystem.open("/mnt/sda1/www/pair.txt", FILE_WRITE);  //re-create the file
    pairFileReset.print(pair);
    pairFileReset.close(); 
    client.println(F("Setup has been successfully updated"));
  }

  if (parameter == "01" && !digitalRead(Y_LED_A) && !digitalRead(Y_LED_B)){
    pair = 01;
    File pairFileReset = FileSystem.open("/mnt/sda1/www/pair.txt", FILE_WRITE);  //re-create the file
    pairFileReset.print(pair);
    pairFileReset.close(); 
    client.println(F("Setup has been successfully updated"));
  }
  
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Configuration Commands                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////

void confCommand(BridgeClient client){
 
  String config_command=client.readStringUntil('/');

  //  Configure Silos Minimum Level
  if(config_command=="limit"){
    String type_limit = client.readStringUntil('/');
    if(type_limit == "L1min"){L1Min=client.parseFloat();}
    if(type_limit == "L2min"){L2Min=client.parseFloat();}
  
    FileSystem.remove("/mnt/sda1/www/limit.txt");
    File limitConf=FileSystem.open("/mnt/sda1/www/limit.txt", FILE_WRITE);
    limitConf.print(L1Min);
    limitConf.print('\n');
    limitConf.print(L2Min);
    limitConf.print('\n');
    limitConf.close();
  }

    //  Configure Email Notification
    if(config_command=="email"){
    FileSystem.remove("/mnt/sda1/www/email.txt");
    File emailConf=FileSystem.open("/mnt/sda1/www/email.txt", FILE_WRITE);
    email_enabled=client.parseInt();
    emailConf.print(email_enabled);
    emailConf.close();
    calls1 = calls2 = 0;
  if(email_enabled){
    client.println(F("Email notification has been enabled"));
  }
  else{
    client.println(F("Email notification has been disabled"));
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Date and Time function                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////
String getTimeStamp() {
  String result;
  Process time;
  // date is a command line utility to get the date and the time
  // in different formats depending on the additional parameter
  time.begin("date");
  time.addParameter("+%D-%T");  // parameters: D for the complete date mm/dd/yy
  //             T for the time hh:mm:ss
  time.run();  // run the command

  // read the output of the command
  while (time.available() > 0) {
    char c = time.read();
    if (c != '\n')
      result += c;
  }
  return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                            Email function                                     //
///////////////////////////////////////////////////////////////////////////////////////////////////

void runSendEmail(byte parameter){
  Serial.println("Sending Email to Laboratory...");
  
  Process p;
  p.begin("python");
  if(parameter == 0x00){
    p.addParameter("/mnt/sda1/www/sendemail1.py");
  }
  if(parameter == 0x01){
    p.addParameter("/mnt/sda1/www/sendemail2.py");
  }
  p.run();
}
