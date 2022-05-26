//Libraries:
#include <Wire.h>


// LED Pin Declerations:
const char keyLEDPin = 2;
const char onBatLEDPin = 3;
const char drillLEDPin = 4;
const char masterFireLEDPin = 5;
const char masterPreAlarmLEDPin = 6;
const char masterFaultLEDPin = 8;
const char masterIsolateLEDPin = 7;
const char zone1FireLEDPin = 9;
const char zone1FaultLEDPin = 10;
const char zone1IsolateLEDPin = 11;
const char zone1AlertLEDPin = 12;
const char zone1EvacLEDPin = 13;
const char zone1PALEDPin = 22;
const char zone1WIPLEDPin = 23;
const char zone1OccLEDPin = 24;
const char zone1ClearLEDPin = 25;

//Inputs Pin Declerations:
const char silenceButPin = 26;
const char acknButPin = 27;
const char cycleButPin = 28;
const char isolateButPin = 29;
const char deIsolateButPin = 30;
const char rstButPin = 31;
const char zone1AlertButPin = 32;
const char zone1EvacButPin = 33;
const char zone1PAButPin = 34;
const char zone1WIPButPin = 35;
const char zone1OccButPin = 36;
const char keyButPin = 37;

const char PIRPin = 52;

//Button States:
bool silenceState;
bool acknState;
bool cycleState;
bool isolateState;
bool deIsolateState;
bool rstState;
bool zone1AlertState;
bool zone1EvacState;
bool zone1PAState;
bool zone1WIPState;
bool zone1OccState;
bool keyState;

//Prev State:
bool prevSilenceState;
bool prevAcknState;
bool prevCycleState;
bool prevIsolateState;
bool prevDeIsolateState;
bool prevRstState;
bool prevZone1AlertState;
bool prevZone1EvacState;
bool prevZone1PAState;
bool prevZone1WIPState;
bool prevZone1OccState;
bool prevKeyState;

//But Toogle States:
bool ToggleSilenceState;
bool TogglevAcknState;
bool ToggleCycleState;
bool ToggleIsolateState;
bool ToggleDeIsolateState;
bool ToggleRstState;
bool ToggleZone1AlertState;
bool ToggleZone1EvacState;
bool ToggleZone1PAState;
bool ToggleZone1WIPState;
bool ToggleZone1OccState;
bool ToggleKeyState;

//Zone Status Information:
int Adress = 0;
bool Fault = false;
bool Alarm = false;
bool preAlarm = false;
bool Smoke = false;
bool Heat = false;
bool Motion = false;
bool Test = false;
bool Motion2 = false;

//Pulse States:
bool pulseFxState;
bool masterFireState;
bool masterFirePulse;
bool masterPreAlarmState;
bool masterPreAlarmPulse;
bool zone1FireState;
bool zone1FirePulse;


//misc:
unsigned long runTime;
unsigned long prevRunTime;
const int buzzerPin = 51;
bool silenceLatch = false;
bool prevAlarmState = false;
bool isolateLatch = false;
bool motionLatch = false;

//Acknoledge:
bool acknLatch = false;

//Ben LCD
const int LCDSmokePin = 40;
const int LCDHeatPin = 41;
const int LCDMotionPin = 42;
const int LCDTestPin = 43;
const int LCDFaultPin = 44;
const int LCDAlarmPin = 45;
const int LCDPreAlarmPin = 46;


void setup() {
  Serial.begin(9600);

  //Initialise LED pin states 1:
  for(int i = 2; i <= 13; i++){
    pinMode(i, OUTPUT);
  }
  
  //Initialise LED pin states 2:
  for(int i = 22; i <= 25; i++){
    pinMode(i, OUTPUT);
  }

  //Initialise Inputs:
  for(int i = 26; i <= 37; i++){
    pinMode(i, INPUT);
  }

  //Initialise Ben LCD:
  for(int i = 40; i <= 46; i++){
    pinMode(i, OUTPUT);
  }

  pinMode(PIRPin,INPUT);
  pinMode(buzzerPin, OUTPUT);

  //Zonal Information:
  Wire.begin(0);
  Wire.onReceive(ZoneRead);

  delay(10);
  panelTest();

}

void loop() {
  runTime = millis();
  prevRunTime;

  keyState = digitalRead(keyButPin);
  topButtonsRead();

  //Manual EWIS Opperations:
  if(keyState == true){
      buttonStateDeclaration(zone1AlertButPin, &zone1AlertState, &prevZone1AlertState, zone1AlertLEDPin, &ToggleZone1AlertState);
      buttonStateDeclaration(zone1EvacButPin, &zone1EvacState, &prevZone1EvacState, zone1EvacLEDPin, &ToggleZone1EvacState);
      buttonStateDeclaration(zone1PAButPin, &zone1PAState, &prevZone1PAState, zone1PALEDPin, &ToggleZone1PAState);
      buttonStateDeclaration(zone1WIPButPin, &zone1WIPState, &prevZone1WIPState, zone1WIPLEDPin, &ToggleZone1WIPState);
      //buttonStateDeclaration(zone1OccButPin, &zone1OccState, &prevZone1OccState, zone1OccLEDPin, &ToggleZone1OccState);
      if(digitalRead(zone1OccButPin) == true){
        digitalWrite(zone1ClearLEDPin,HIGH);
      }
  }

  //Alert and Evac Toggle:
  if(ToggleZone1AlertState == true && ToggleZone1EvacState == true){
    ToggleZone1AlertState = false;
  }

  isolateZone();
  acknAlert();
  benLCD();

  //silence buzzer:
  if(silenceState == true){
    noTone(buzzerPin);
    silenceLatch = true;
  }

  //read sensor info and light up board:
  readLightUpBoard();
  if(Fault == true){
    digitalWrite(zone1FaultLEDPin, HIGH);
    digitalWrite(masterFaultLEDPin, HIGH);
  }
  else{
    digitalWrite(zone1FaultLEDPin, LOW);
    digitalWrite(masterFaultLEDPin, LOW);
  }
  pulseFx();
  pulseStates();
}


void buttonStateDeclaration(int buttonPin, bool* buttonState , bool* previousButtonState, int LEDPin, bool* on){

  *buttonState = digitalRead(buttonPin);

  if (*buttonState == HIGH && *previousButtonState != *buttonState)
  {
    if (*on == false)
    {
      *on = true;
    }
    else
    {
      *on = false;
    }
  }

  if (*on == true)
  {
    digitalWrite(LEDPin, HIGH);
  }
  else
  {
    digitalWrite(LEDPin, LOW);
  }

  *previousButtonState = *buttonState;

}

void benLCD(){
  digitalWrite(LCDSmokePin, Smoke);
  digitalWrite(LCDHeatPin, Heat);
  digitalWrite(LCDMotionPin, Motion2);
  digitalWrite(LCDTestPin, Test);
  digitalWrite(LCDFaultPin, Fault);
  digitalWrite(LCDAlarmPin, Alarm);
  digitalWrite(LCDPreAlarmPin, preAlarm);
  
}

void topButtonsRead(){
  //buttons can only be pressed when key is inserted:
  if(keyState == true){
    silenceState = digitalRead(silenceButPin);
    acknState = digitalRead(acknButPin);
    cycleState = digitalRead(cycleButPin);
    isolateState = digitalRead(isolateButPin);
    deIsolateState = digitalRead(deIsolateButPin);
    rstState = digitalRead(rstButPin);
    digitalWrite(keyLEDPin, HIGH);
  }
  else{
    digitalWrite(keyLEDPin, LOW);
  }
}

void ZoneRead(int howMany){
  Adress = Wire.read();
  Fault = Wire.read();
  Alarm = Wire.read();
  preAlarm = Wire.read();
  Smoke = Wire.read();
  Heat = Wire.read();
  Motion = Wire.read();
  Test = Wire.read();


  Serial.print("Adr=");
  Serial.print(Adress);         
  Serial.print(" Fault=");
  Serial.print(Fault);
  Serial.print(" Alarm=");
  Serial.print(Alarm);
  Serial.print(" preAl=");
  Serial.print(preAlarm);
  Serial.print(" Smoke=");
  Serial.print(Smoke);
  Serial.print(" Heat=");
  Serial.print(Heat);
  Serial.print(" Motion");
  Serial.print(Motion);
  Serial.print(" Test=");
  Serial.println(Test);

}

void readLightUpBoard(){

  //people
  Motion2 = digitalRead(PIRPin);
  if(Motion2 == true){
    digitalWrite(zone1OccLEDPin,HIGH);
  }
  else if(Motion2 == false && motionLatch != true){
    digitalWrite(zone1OccLEDPin,LOW);
  }

  //motion latch:
  if(Alarm == true || preAlarm == true){
    if (Motion2 == true){
      digitalWrite(zone1OccLEDPin,HIGH);
      motionLatch = true;
    }
  }

  //Alarm:
  if(Alarm == true && preAlarm == false){
    if(isolateLatch == false){
      masterFireState = true;
      zone1FireState = true;
      masterPreAlarmState = false;
      if(silenceLatch == false || prevAlarmState == false){
        tone(buzzerPin, 880);
      }
      prevAlarmState = true;

      //EWIS:
      if(keyState == false){
        digitalWrite(zone1AlertLEDPin, HIGH);
        ToggleZone1AlertState = true;
      
      }
    }
    else if(isolateLatch == true){
      digitalWrite(zone1FireLEDPin, HIGH);
    }
  }

  //Pre Alarm:
  else if(preAlarm == true && Alarm == false){
    if(isolateLatch == false){
      masterPreAlarmState = true;
      zone1FireState = true;
    }

    else if(isolateLatch == true){
      digitalWrite(zone1FireLEDPin, HIGH);
    }
  }

  if(rstState == true){
    masterFireState = false;
    zone1FireState = false;
    masterPreAlarmState = false;

    panelTest();
  }

  
}

void pulseStates(){

  //masterFireStateFlash:
  if(masterFireState == true){
    if(pulseFxState == true){
      digitalWrite(masterFireLEDPin, HIGH);
    }
    else if(pulseFxState == false && acknLatch == false){
      digitalWrite(masterFireLEDPin, LOW);
    }
  }
  else{
    digitalWrite(masterFireLEDPin, LOW);
  }

  //Zone 1 fire pulse:
  if(zone1FireState == true){
    if(pulseFxState == true){
      digitalWrite(zone1FireLEDPin, HIGH);
    }
    else if(pulseFxState == false && acknLatch == false){
      digitalWrite(zone1FireLEDPin, LOW);
    }
  }
  else{
    digitalWrite(zone1FireLEDPin, LOW);
  }

  //Pre Alarm fire pulse:
  if(masterPreAlarmState == true){
    if(pulseFxState == true){
      digitalWrite(masterPreAlarmLEDPin, HIGH);
    }
    else if(pulseFxState == false && acknLatch == false){
      digitalWrite(masterPreAlarmLEDPin, LOW);
    }
  }
  else{
    digitalWrite(masterPreAlarmLEDPin, LOW);
  }

}

void pulseFx(){

  if(runTime > (prevRunTime + 500)){
    pulseFxState = !pulseFxState;
    prevRunTime = runTime;
  }
}

void isolateZone(){
  //Isolate Zone 1:
  if(isolateState == true){
    isolateLatch = true;
    digitalWrite(zone1IsolateLEDPin, HIGH);
    digitalWrite(masterIsolateLEDPin, HIGH);
  }

  //De isolate zone 1:
  if(deIsolateState == true){
    isolateLatch = false;
    digitalWrite(zone1IsolateLEDPin, LOW);
    digitalWrite(masterIsolateLEDPin, LOW);
  }
}

void acknAlert(){
  //switchon:
  if(acknState == HIGH && acknState != prevAcknState){
    acknLatch = true;
    Alarm = false;
    preAlarm = false;
  }

  prevAcknState = acknState;
}


void panelTest(){
  //All lights on:
  delay(500);
  for(int i = 2; i <= 13; i++){
    digitalWrite(i,HIGH);
  }

  for(int i = 22; i <= 25; i++){
    digitalWrite(i,HIGH);
  }

  delay(1000);

  //All lights off:
  for(int i = 2; i <= 13; i++){
    digitalWrite(i,LOW);
  }

  for(int i = 22; i <= 25; i++){
    digitalWrite(i,LOW);
  }

  //Reset values:
  silenceLatch = false;
  prevAlarmState = false;

  ToggleSilenceState = false;
  TogglevAcknState = false;
  ToggleCycleState = false;
  ToggleIsolateState = false;
  ToggleDeIsolateState = false;
  ToggleRstState = false;
  ToggleZone1AlertState = false;
  ToggleZone1EvacState = false;
  ToggleZone1PAState = false;
  ToggleZone1WIPState = false;
  ToggleZone1OccState = false;
  ToggleKeyState = false;

  Alarm = false;
  preAlarm = false;
  isolateLatch = false;
  acknLatch = false;
  motionLatch = false;
  Fault = false;

}
