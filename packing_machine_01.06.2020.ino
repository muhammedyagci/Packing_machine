#include "HX711.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f, 16, 2);
HX711 scale;

unsigned long int timeOut, switchControlTimeOut;

float readScales;
bool startSystem = false, stopSystem = true, manual = true;
int scalesMode = 1, setScales;


const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;
const int conveyorMotorR = 5;
const int conveyorMotorL = 6;
const int startButton = 7;
const int configButton = 8;
const int stopButton = 9;
const int scalesMotorR = 10;
const int scalesMotorL = 11;
const int topSwitch = 12;
const int bottomSwitch = 13;

void configScale(int setScalesMode) {

  switch (setScalesMode) {
    case 1:
      manual = true;
      setScales = 200;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Program Modu : 1");
      lcd.setCursor(0, 1);
      lcd.print("Paket: 200 gr(M)");
      break;

    case 2:
      manual = false;
      setScales = 200;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Program Modu : 2");
      lcd.setCursor(0, 1);
      lcd.print("Paket: 200 gr(O)");
      break;

    case 3:
      manual = true;
      setScales = 400;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Program Modu : 3");
      lcd.setCursor(0, 1);
      lcd.print("Paket: 400 gr(M)");
      break;

    case 4:
      manual = false;
      setScales = 400;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Program Modu : 4");
      lcd.setCursor(0, 1);
      lcd.print("Paket: 400 gr(O)");
      break;

    case 5:
      manual = true;
      setScales = 500;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Program Modu : 5");
      lcd.setCursor(0, 1);
      lcd.print("Paket: 500 gr(M)");
      break;

    case 6:
      manual = false;
      setScales = 500;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Program Modu : 6");
      lcd.setCursor(0, 1);
      lcd.print("Paket: 500 gr(O)");
      break;

    case 7:
      manual = true;
      setScales = 1000;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Program Modu : 7");
      lcd.setCursor(0, 1);
      lcd.print("Paket: 1000gr(M)");
      break;

    case 8:
      manual = false;
      setScales = 1000;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Program Modu : 8");
      lcd.setCursor(0, 1);
      lcd.print("Paket: 1000gr(O)");
      break;

    case 9:
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("TARTIM");
      lcd.setCursor(6, 1);
      lcd.print("MODU");

      break;

    default:
      break;
  }

}

void systemFailed() {


  digitalWrite(conveyorMotorR, LOW);
  digitalWrite(conveyorMotorL, LOW);
  digitalWrite(scalesMotorR, LOW);
  digitalWrite(scalesMotorL, LOW);

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("MOTOR / SWITCH");
  lcd.setCursor(4, 1);
  lcd.print("ARIZASI");
  delay(3000);
  while (1);


}

void dara_alma() {

  scale.set_scale(2415.f);
  scale.tare();

  while (!scale.is_ready());
  readScales = scale.get_units(1) * 10;
  scale.power_down();
  delay(500);
  scale.power_up();

}

void systemStart() {

  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("SISTEM");
  lcd.setCursor(2, 1);
  lcd.print("BASLATILIYOR");
  analogWrite(scalesMotorR, 255);
  switchControlTimeOut = millis();
  while (!digitalRead(topSwitch) && !((millis() - switchControlTimeOut) > 3000));
  if ((millis() - switchControlTimeOut) > 3000) {
    systemFailed();
  }
  delay(10);
  analogWrite(scalesMotorR, 0);
  delay(10);

  dara_alma();

  delay(10);
  if (scalesMode != 9) {
    analogWrite(conveyorMotorR, 255);
  }

}

void systemStop() {


  digitalWrite(conveyorMotorR, LOW);
  digitalWrite(conveyorMotorL, LOW);
  digitalWrite(scalesMotorR, LOW);
  digitalWrite(scalesMotorL, LOW);

  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("SISTEN");
  lcd.setCursor(3, 1);
  lcd.print("DURDURULDU");
  delay(3000);
  configScale(scalesMode);


}

void setup() {

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  lcd.init();
  lcd.backlight();

  delay(1000);

  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("SISTEM");
  lcd.setCursor(4, 1);
  lcd.print("ACILIYOR");
  delay(3000);
  configScale(scalesMode);

  scale.set_scale(2415.f);
  scale.tare();

  pinMode(startButton, INPUT);
  pinMode(configButton, INPUT);
  pinMode(stopButton, INPUT);
  pinMode(topSwitch, INPUT);
  pinMode(bottomSwitch, INPUT);

  pinMode(conveyorMotorR, OUTPUT);
  pinMode(conveyorMotorL, OUTPUT);
  pinMode(scalesMotorR, OUTPUT);
  pinMode(scalesMotorL, OUTPUT);

  digitalWrite(conveyorMotorR, LOW);
  digitalWrite(conveyorMotorL, LOW);
  digitalWrite(scalesMotorR, LOW);
  digitalWrite(scalesMotorL, LOW);



}

void loop() {

  timeOut = millis();

  while (!scale.is_ready()) {
    if ((millis() - timeOut) > 10000) {
      startSystem = false;
      systemStop();
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("SENSOR ARIZASI");
      while (!scale.is_ready());
      configScale(scalesMode);
    }
  }
  readScales = scale.get_units(1) * 10;
  if (readScales < 0) {
    readScales = 0.0;
  }


  if (startSystem) {

    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("SISTEM AKTIF");

    if (scalesMode == 9) {
      lcd.setCursor(0, 1);
      lcd.print("TARTIM = ");
      lcd.setCursor(9, 1);
      lcd.print(String((int)readScales) + " gr");
    } else {
      if (manual) {
        lcd.setCursor(0, 1);
        lcd.print(String((int)readScales));
        lcd.setCursor(4, 1);
        lcd.print("gr/" + String(setScales) + "gr(M)");
      } else {
        lcd.setCursor(0, 1);
        lcd.print(String((int)readScales));
        lcd.setCursor(4, 1);
        lcd.print("gr/" + String(setScales) + "gr(O)");
      }
    }

    if (scalesMode == 1 || scalesMode == 2) {
      if (readScales < 100.0) {
        analogWrite(conveyorMotorR, 255);
      } else if (readScales >= 100.0 && readScales < 200.0) {
        analogWrite(conveyorMotorR, 100);
      } else {
        analogWrite(conveyorMotorR, 0);
        delay(100);
        if (manual) {
          while (!digitalRead(configButton));
          delay(10);
        }
        analogWrite(scalesMotorL, 200);
        switchControlTimeOut = millis();
        while (!digitalRead(bottomSwitch) && !((millis() - switchControlTimeOut) > 3000));
        if ((millis() - switchControlTimeOut) > 3000) {
          systemFailed();
        }
        delay(10);
        analogWrite(scalesMotorL, 0);
        delay(500);
        analogWrite(scalesMotorR, 200);
        switchControlTimeOut = millis();
        while (!digitalRead(topSwitch) && !((millis() - switchControlTimeOut) > 3000));
        if ((millis() - switchControlTimeOut) > 3000) {
          systemFailed();
        }
        delay(10);
        analogWrite(scalesMotorR, 0);
        delay(100);

        dara_alma();

        analogWrite(conveyorMotorR, 255);
      }
    } else if (scalesMode == 3 || scalesMode == 4) {
      if (readScales < 300.0) {
        analogWrite(conveyorMotorR, 255);
      } else if (readScales >= 300.0 && readScales < 400.0) {
        analogWrite(conveyorMotorR, 100);
      } else {
        analogWrite(conveyorMotorR, 0);
        delay(100);
        if (manual) {
          while (!digitalRead(configButton));
          delay(10);
        }
        analogWrite(scalesMotorL, 200);
        switchControlTimeOut = millis();
        while (!digitalRead(bottomSwitch) && !((millis() - switchControlTimeOut) > 3000));
        if ((millis() - switchControlTimeOut) > 3000) {
          systemFailed();
        }
        delay(10);
        analogWrite(scalesMotorL, 0);
        delay(500);
        analogWrite(scalesMotorR, 200);
        switchControlTimeOut = millis();
        while (!digitalRead(topSwitch) && !((millis() - switchControlTimeOut) > 3000));
        if ((millis() - switchControlTimeOut) > 3000) {
          systemFailed();
        }
        delay(10);
        analogWrite(scalesMotorR, 0);
        delay(100);

        dara_alma();

        analogWrite(conveyorMotorR, 255);
      }
    } else if (scalesMode == 5 || scalesMode == 6) {
      if (readScales < 400.0) {
        analogWrite(conveyorMotorR, 255);
      } else if (readScales >= 400.0 && readScales < 500.0) {
        analogWrite(conveyorMotorR, 100);
      } else {
        analogWrite(conveyorMotorR, 0);
        delay(100);
        if (manual) {
          while (!digitalRead(configButton));
          delay(10);
        }
        analogWrite(scalesMotorL, 200);
        switchControlTimeOut = millis();
        while (!digitalRead(bottomSwitch) && !((millis() - switchControlTimeOut) > 3000));
        if ((millis() - switchControlTimeOut) > 3000) {
          systemFailed();
        }
        delay(10);
        analogWrite(scalesMotorL, 0);
        delay(500);
        analogWrite(scalesMotorR, 200);
        switchControlTimeOut = millis();
        while (!digitalRead(topSwitch) && !((millis() - switchControlTimeOut) > 3000));
        if ((millis() - switchControlTimeOut) > 3000) {
          systemFailed();
        }
        delay(10);
        analogWrite(scalesMotorR, 0);
        delay(100);

        dara_alma();

        analogWrite(conveyorMotorR, 255);
      }
    } else if (scalesMode == 7 || scalesMode == 8) {
      if (readScales < 900.0) {
        analogWrite(conveyorMotorR, 255);
      } else if (readScales >= 900.0 && readScales < 1000.0) {
        analogWrite(conveyorMotorR, 100);
      } else {
        analogWrite(conveyorMotorR, 0);
        delay(100);
        if (manual) {
          while (!digitalRead(configButton));
          delay(10);
        }
        analogWrite(scalesMotorL, 200);
        switchControlTimeOut = millis();
        while (!digitalRead(bottomSwitch) && !((millis() - switchControlTimeOut) > 3000));
        if ((millis() - switchControlTimeOut) > 3000) {
          systemFailed();
        }
        delay(10);
        analogWrite(scalesMotorL, 0);
        delay(500);
        analogWrite(scalesMotorR, 200);
        switchControlTimeOut = millis();
        while (!digitalRead(topSwitch) && !((millis() - switchControlTimeOut) > 3000));
        if ((millis() - switchControlTimeOut) > 3000) {
          systemFailed();
        }
        delay(10);
        analogWrite(scalesMotorR, 0);
        delay(100);

        dara_alma();

        analogWrite(conveyorMotorR, 255);
      }
    } else if (scalesMode == 9) {


    } else {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("YAZILIM HATASI");
    }






  }

  if (digitalRead(configButton) && !startSystem) {
    delay(50);
    scalesMode++;
    while (digitalRead(configButton));
    if (scalesMode == 10) {
      scalesMode = 1;
    }
    configScale(scalesMode);
  }

  if (digitalRead(configButton) && startSystem && manual) {

  }


  if (digitalRead(startButton) && !startSystem) {
    delay(10);
    stopSystem = false;
    startSystem = true;
    systemStart();
  }

  if (digitalRead(stopButton) && !stopSystem) {
    delay(10);
    stopSystem = true;
    startSystem = false;
    systemStop();
  }

}
