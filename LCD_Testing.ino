#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); 

bool smoke = false;
bool alarm = false;
bool preAlarm = false;
bool heat = false;
bool motion = false;
bool test = false;
bool fault = false;

char alarmState[11];
int address = 2;
int i;

bool previousSmoke = false;
bool previousAlarm = false;
bool previousPreAlarm = false;
bool previousHeat = false;
bool previousMotion = false;
bool previousTest = false;
bool previousFault = false;



void setup() {

  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();

  for (i = 2; i < 9; i++)
  {
    pinMode(i, INPUT);
  }

}

void loop() {

  smoke = digitalRead(2);
  heat = digitalRead(3);
  motion = digitalRead(4);
  test = digitalRead(5);
  fault = digitalRead(6);

  alarm = digitalRead(7);
  preAlarm = digitalRead(8);

  if (previousAlarm != alarm ||  previousHeat != heat || previousMotion != motion || 
  previousTest != test || previousFault != fault || previousAlarm != alarm || 
  previousPreAlarm != preAlarm)
  {
    lcd.clear();
  }
  
  printValues(smoke, heat, motion, test, fault);
  
  if (alarm == LOW && preAlarm == LOW)
  {
    lcd.setCursor(2,2);
    lcd.print("System is ready");
  }
  
  else
  {
  lcd.setCursor(0,0);
  lcd.print("Zone 3");
  
  lcd.setCursor(0,1);
  lcd.print("Status: ");
  alarmCheck(alarm, preAlarm);
  lcd.print(alarmState);

  lcd.setCursor(0,2);
  lcd.print("Sensor Address: ");
  /* for (i = 15; i < 20; i++)
  {
    lcd.setCursor(i,2);
    lcd.print(" ");
  } */
  lcd.setCursor(16,2);
  lcd.print(address);

  /* for (i = 0; i < 20; i++)
  {
    lcd.setCursor(i,3);
    lcd.print(" ");
  }  */
  printDetection(smoke, heat, motion, test, fault);
 
  }
 
  previousSmoke = smoke;
  previousHeat = heat;
  previousMotion = motion;
  previousTest = test;
  previousFault = fault;
  previousAlarm = alarm;
  previousPreAlarm = preAlarm;

  delay(200);

}

void alarmCheck(bool alarm, bool preAlarm){
  
  if (alarm == true)
  {
    strcpy(alarmState, "Alarm");
  }
  
  else if (preAlarm == true)
  {
    strcpy(alarmState, "Pre-Alarm");
  }
  
  else 
  {
    strcpy(alarmState, "No Alarm");  
  } 
}

void printDetection(bool smoke, bool heat, bool motion, bool test, bool fault){
  
  if (smoke == true)
  {
    lcd.setCursor(0,3);
    lcd.print("SMK");
  }

  if (heat == true)
  {
    lcd.setCursor(4,3);
    lcd.print("THM");
  }

  if (motion == true)
  {
    lcd.setCursor(8,3);
    lcd.print("MOT");
  }

  if (test == true)
  {
    lcd.setCursor(12,3);
    lcd.print("TST");
  }

  if (fault == true)
  {
    lcd.setCursor(16,3);
    lcd.print("FLT");
  }

}

void printValues(bool smoke, bool heat, bool motion, bool test, bool fault)
{

  Serial.print("Alarm:");
  Serial.print(alarm);
  Serial.print("\t");
  
  Serial.print("Pre-Alarm:");
  Serial.print(preAlarm);
  Serial.print("\t");

  Serial.print("Smoke:");
  Serial.print(smoke);
  Serial.print("\t");
  
  Serial.print("heat:");
  Serial.print(heat);
  Serial.print("\t");
  
  Serial.print("motion:");
  Serial.print(motion);
  Serial.print("\t");
  
  Serial.print("test:");
  Serial.print(test);
  Serial.print("\t");
  
  Serial.print("fault:");
  Serial.print(fault);
  Serial.print("\t");

  Serial.print("\n");
}
