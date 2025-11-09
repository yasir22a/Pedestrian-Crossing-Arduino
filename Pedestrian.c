#include <LiquidCrystal_I2C.h>  // Include LCD library

LiquidCrystal_I2C lcd(0x26, 16, 2);  // Create LCD object (I2C address, columns, rows)

int CAR_RED = 12;    // vehicles wait
int CAR_YELLOW = 11; // vehicles prepare
int CAR_GREEN = 10;  // vehicles go
int PED_RED = 9;     // pedestrian wait
int PED_GREEN = 8;   // pedestrian go
int BUZZER = 7;      // buzzer for sound
int IR_SENSOR = 2;   // IR sensor for automatic detection

// A push button is added in parallel with the IR sensor for manual triggering or backup control.

int count = 0;       // variable to count pedestrians

void setup() {
  // Define pin modes for all components
  pinMode(CAR_RED, OUTPUT);
  pinMode(CAR_YELLOW, OUTPUT);
  pinMode(CAR_GREEN, OUTPUT);
  pinMode(PED_RED, OUTPUT);
  pinMode(PED_GREEN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(IR_SENSOR, INPUT_PULLUP);  // use internal pull-up resistor

  lcd.init();        // Initialize LCD
  lcd.backlight();   // Turn on LCD backlight

  Serial.begin(9600);  // Start serial communication
  Serial.println("System Ready with Pull-up Enabled");

  // Display initial message on LCD
  lcd.setCursor(3, 0);
  lcd.print("Pedestrian");
  lcd.setCursor(0, 1);
  lcd.print("Count: 0");
}

void loop() {
  // Read IR sensor state
  int sensorState = digitalRead(IR_SENSOR);

  Serial.print("IR Sensor State: ");
  Serial.println(sensorState == LOW ? "Detected" : "No Object");

  if (sensorState == LOW) {  // pedestrian detected
    count++;  // increase pedestrian count
    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.print(count);
    //lcd.print("   "); // clear extra digits
    delay(300);

    // STEP 1: Car goes from GREEN → YELLOW → RED
    digitalWrite(CAR_GREEN, LOW);   // turn off car green
    digitalWrite(CAR_YELLOW, HIGH); // turn on car yellow (warning)
    delay(2000);                    // wait for 2 seconds
    digitalWrite(CAR_YELLOW, LOW);  // turn off yellow
    digitalWrite(CAR_RED, HIGH);    // turn on car red (stop vehicles)

    // STEP 2: Pedestrian Green ON
    digitalWrite(PED_RED, LOW);     // turn off pedestrian red
    digitalWrite(PED_GREEN, HIGH);  // turn on pedestrian green (allow to cross)

    // STEP 3: Slow beep for 5 sec
    for (int i = 0; i < 5; i++) {
      tone(BUZZER, 1000);  // 1000hz
      delay(300);
      noTone(BUZZER);      // stop sound
      delay(700);
    }

    // STEP 4: Fast beep for last 3 sec
    for (int i = 0; i < 5; i++) {
      tone(BUZZER, 1000);
      delay(150);
      noTone(BUZZER);
      delay(150);
    }

    // STEP 5: Back to normal state
    digitalWrite(PED_GREEN, LOW);   // turn off pedestrian green
    digitalWrite(PED_RED, HIGH);    // pedestrian red ON (stop crossing)

    digitalWrite(CAR_RED, LOW);     // turn off car red
    digitalWrite(CAR_YELLOW, HIGH); // car yellow ON (prepare)
    delay(2000);                    // wait 2 seconds
    digitalWrite(CAR_YELLOW, LOW);  // car yellow OFF
    digitalWrite(CAR_GREEN, HIGH);  // car green ON (vehicles go)
  } 
  else {
    // Normal state (cars go, pedestrian wait)
    digitalWrite(CAR_GREEN, HIGH);  // car green ON
    digitalWrite(CAR_RED, LOW);     // car red OFF
    digitalWrite(CAR_YELLOW, LOW);  // car yellow OFF
    digitalWrite(PED_RED, HIGH);    // pedestrian red ON
    digitalWrite(PED_GREEN, LOW);   // pedestrian green OFF
  }

  delay(100);  // small delay to stabilize readings
}
