#include <Adafruit_LiquidCrystal.h>
#include <Keypad.h>

Adafruit_LiquidCrystal lcd(0);

// Use 4 rows and 3 columns
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {2, 3, 4, 5}; // Keypad Rows → Arduino pins 2–5
byte colPins[COLS] = {6, 7, 8};    // Keypad Cols (1–3) → Arduino pins 6–8

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


const int sensorPin = A0; // sensor pin
int reading;   // ADC reading
float volts;   // voltage
float temperatureC;    // temp in deg C


// fan pins
const int fanPin = 9 ;
const int buzzerPin = 10 ;

int mode = 6; // Start in Auto mode

// actual analog reference voltage (measure with multimeter)
const float aref_voltage = 1.07;

void setup() {
  
  // for Fan 
  pinMode(fanPin,OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // use internal 1.1V reference voltage
  analogReference(INTERNAL);
  
  // For fan Speed Display 
  lcd.begin(16, 2);
  lcd.setBacklight(1);
  
  lcd.setCursor(4, 0);
  lcd.print("Welcome");
  delay(1000);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(" to Pavan's ");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Auto Temperature");
  delay(2000);
  lcd.clear();
  lcd.print("Auto Mode : 6");
  delay(2000);
  lcd.clear();
  lcd.print("+Manual Override");
  delay(2000);
  lcd.clear();
  lcd.print("Manual:1,2,3,4,5");
  delay(2000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Let's start");
  delay(2000);
  lcd.clear();
  
  // initialize serial communication
  Serial.begin(9600);
}

void loop() {
  
  
  // put your main code here, to run repeatedly:
  reading = analogRead(A0);  // read analog pin
  volts = reading * aref_voltage/ 1023.0; // convert to voltage 
  temperatureC = (volts - 0.5)*100.0;  // convert to deg C
  
  char key = keypad.getKey();
	if (key) {
  	if (key >= '0' && key <= '6') {
    	mode = key - '0';
    	Serial.print("Mode set to: ");
    	Serial.println(mode);
 	 }
	}

  
  // Determine fan speed (PWM 0–255)
  int fanSpeed = 0;
  if(mode == 6) {
  if (temperatureC < 15 ) {
    fanSpeed = 0;
  } else if (temperatureC < 20) {
    fanSpeed = 51;
  } else if (temperatureC < 25) {
    fanSpeed = 102;
  }else if (temperatureC < 30) {
    fanSpeed = 153;
  }else if (temperatureC < 35) {
    fanSpeed = 204;
  } else {
    fanSpeed = 255;
  }
  } else if (mode == 0) {
    fanSpeed = 0; // Manual Off
  } else {
    // Manual override modes
    fanSpeed = map(mode, 1, 5, 51, 255); // Mode 1 = min , Mode 5 = max
  }
    
  analogWrite(fanPin, fanSpeed); // Set fan speed
  

	 if (temperatureC > 40 && mode >= 1 && mode < 5) {
    tone(buzzerPin, 1000); // Continuous buzzer alert
  } else {
    noTone(buzzerPin);
  }


  // Display on LCD
  lcd.setCursor(0, 0);
  // lcd.print("Temp: ");
  lcd.print(temperatureC, 1);
  lcd.print("C ");

  lcd.setCursor(6, 0);
  if (mode == 6) {
    lcd.print("Mode: Auto");
  } else if (mode == 0) {
    lcd.print("Mode: Off");
  } else {
    lcd.print("Man: ");
    lcd.print(mode);
    // lcd.print("     ");
  }; // clear trail
  
  lcd.setCursor(0, 1);
  lcd.print("Fan PWM: ");
  lcd.print(fanSpeed);

  // print data
  Serial.print("ADC reading: ");
  Serial.print(reading);
  Serial.print(" Volts: ");
  Serial.print(volts);
  Serial.print(" Degrees C: ");
  Serial.print(temperatureC);
  Serial.print(" C | Mode: ");
  Serial.print(mode);
  Serial.print("  FanSpeed: ");
  Serial.println(fanSpeed);

  delay(500);
} 