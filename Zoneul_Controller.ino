#include <Wire.h>
                                                                                    
int sensorCLK = 9;
int sensorDATA = 10;
int sensorCLR = 4;
int maxSensorValue = 5;
int address = 0;
int GATE = HIGH;
int alarmsendPin = 5;
int gatePin = 3;
int vtPin = 2;
int smokePin = 4;
int thermalPin = 6;
int pirPin = 7;
int testPin = 8;
int timeconstant = 100;

bool alarm = false;
bool preAlarm = false;
bool smoke = false;
bool heat = false;
bool motion = false;
bool test = false;
bool fault = false; 

void setup() {

  Wire.begin();
  
  Serial.begin(9600);
  pinMode(sensorCLK, OUTPUT);
  pinMode(sensorDATA, OUTPUT);
  pinMode(sensorCLR, OUTPUT);
  pinMode(gatePin, OUTPUT);
  pinMode(alarmsendPin, OUTPUT);
  pinMode(vtPin, INPUT);
  pinMode(smokePin, INPUT);
  pinMode(thermalPin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(testPin, INPUT);
  // pinMode();
}

void loop() {
  
  shiftOut(sensorDATA, sensorCLK, MSBFIRST, address);
  Serial.print("Address:");
  Serial.print(address);
  Serial.print("\n");
  digitalWrite(alarmsendPin, LOW);
  delay(timeconstant);
  GATE = LOW;

  if (digitalRead(vtPin) == HIGH)
  {
    fault = false;
  }

  delay(timeconstant);

  if (digitalRead(vtPin) == LOW) //In future, need to detect for low for how long, idk
  {
    Serial.println("Fault");
    fault = true;
    // Send Declan fault for LED
  }
  
  GATE = HIGH;
  delay(timeconstant);
  
  sensorRead();
  // sendData();
  
  if (address < maxSensorValue)
  {
    address++;
  }
  if (address == maxSensorValue)
  {
    address = 0;
  }

}

void sensorRead(){
  
  int smoke1 = digitalRead(smokePin);
  int test1 = digitalRead(testPin);
  int thermal1 = digitalRead(thermalPin);
  int pir1 = digitalRead(pirPin);


  Serial.print("Smoke:");
  Serial.print(smoke1);
  Serial.print("\t");
  Serial.print("Thermal:");
  Serial.print(thermal1);
  Serial.print("\t");
  Serial.print("PIR:");
  Serial.print(pir1);
  Serial.print("\t");
  Serial.print("Test:");
  Serial.println(test1);

  
    if ((smoke1 == HIGH && thermal1 == LOW) && (test1 == LOW))
    {
      Serial.print("Pre-Alarm");
      Serial.print("\t");
      smoke = true;
      heat = false;
      preAlarm = true;
      sendData();
      // Send address data to Declan, digitalwrite for LED On chassis
      // HIGH output to LEDs
    }
    
    else
    {
      smoke = false;
      heat = false;
      preAlarm = false; 
    }

    if ((smoke1 == LOW && thermal1 == HIGH) && (test1 == LOW))
    {
      Serial.print("Pre-Alarm");
      heat = true;
      preAlarm = true;
      sendData();
    }
    
    else
    {
      heat = false;
      preAlarm = false;
    }

    
    if (smoke1 == HIGH && thermal1 == HIGH)
    {
      Serial.print("Alarm");
      smoke = true;
      heat = true;
      alarm = true;
      // There is a fire bro
      digitalWrite(alarmsendPin, HIGH); // Turns on LED pin for fire, used for visual inspections
      sendData();
    }
    
    else
    {
      smoke = false;
      heat = false;
      alarm = false;
    }
    
    if (digitalRead(testPin) == HIGH)
    {
      Serial.print("Alarm");
      alarm = true;
      test = true;
      digitalWrite(alarmsendPin, HIGH);
      sendData();
    }
    
    else
    {
      alarm = false;
      test = false;
    }
    
}
void sendData(){

  Serial.print("sendData is succesful");
  Serial.print("\t");

  Serial.print("Fault Value:");
  Serial.print(fault);
  Serial.print("\t");
  
  Serial.print("Alarm Value:");
  Serial.print(alarm);
  Serial.print("\t");
  
  Serial.print("Pre-Alarm Value:");
  Serial.print(preAlarm);
  Serial.print("\t");

  Serial.print("\n");

  Serial.print("Smoke Value:");
  Serial.print(smoke);
  Serial.print("\t");

  Serial.print("Heat Value:");
  Serial.print(heat);
  Serial.print("\t");

  Serial.print("Motion Value:");
  Serial.print(motion);
  Serial.print("\t");

  Serial.print("Test Value:");
  Serial.print(test);
  Serial.print("\t");

  Serial.print("\n");
  
  Wire.beginTransmission(0);
  Wire.write(address);
  Wire.write(fault);
  Wire.write(alarm);
  Wire.write(preAlarm);
  Wire.write(smoke);
  Wire.write(heat);
  Wire.write(motion);
  Wire.write(test);
  Wire.endTransmission();
  
}
