char receivedChar;
boolean newData = false;

//#include <LiquidCrystal.h>
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // select the pins used on the LCD panel


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("");
}

void loop() {
  recvOneChar();
  showNewData();
}

void recvOneChar() {
  if (Serial1.available() > 0) {
    receivedChar = Serial1.read();
//    lcd.print(receivedChar);
    newData = true;
  }
}

void showNewData() {
  if (newData == true) {
    //Serial.print("This just in … ");
    Serial.print(receivedChar);
//    lcd.setCursor(10,1);
    newData = false;
  }
}