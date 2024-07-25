#include <SD.h>
#include <SPI.h>

#define sensor1 A0
#define sensor2 A1
#define sensor3 A2
#define sensor4 A3
#define sensor5 A4 // Arduino pin connected to the AOUT pin of the moisture sensor
#define THRESHOLD 500 // Change your threshold here

const int chipSelect = 10;
const int waterPin = 8;
const int buzzerPin = 6; // Buzzer to Arduino pin 6
const int buttonPin = 4;


char sensorOneCalibration[] = "dataOne.txt";
char charRead;

int sensorDisconnected = 0 //needs actual value

int currentValue = 0;
int wet = 0;
int dry[5];
int x = 0;
int buttonState = 0; // Variable for reading the pushbutton status
int desiredWaterContent = 50;

float correctionValue = 0;

void setup() {
  pinMode(waterPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  Serial.begin(9600);
  Serial.println("Simple SD Card Demo");

  if (SD.begin(chipSelect)) {
    Serial.println("SD card is present & ready");
  } else {
    Serial.println("SD card missing or failure");
    while (1); // Wait here forever
  }

  readDataFromFile(sensorOneCalibration, wet, correctionValue);
  Serial.print("WET VALUE: ");
  Serial.println(wet);
  Serial.print("CORRECTION VALUE: ");
  Serial.println(correctionValue);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  int value = analogRead(sensor1); // Read the analog value from sensor

  if (buttonState == HIGH) {
    calibrate();
  }

  currentValue = analogRead(sensor1);
  int data = currentValue - wet;
  int realTimeValue = 100 - (data * correctionValue);
  Serial.print("HERE: ");
  Serial.println(realTimeValue);

  if (desiredWaterContent > realTimeValue) {
    digitalWrite(waterPin, HIGH);
    delay(5000);
    digitalWrite(waterPin, LOW);
    delay(10000);
  }

  
}

void calibrate() {
  Serial.println("Put all in dry soil");

  tone(buzzerPin, 1000);
  delay(1000);
  noTone(buzzerPin);

  delay(100000);

  dry[1] = analogRead(sensor1);
  dry[2] = analogRead(sensor2);
  dry[3] = analogRead(sensor3);
  dry[4] = analogRead(sensor4);
  dry[5] = analogRead(sensor5); // Read the analog value from sensor for dry condition
  tone(buzzerPin, 1000);
  delay(1000);
  noTone(buzzerPin);

  delay(100000);


  wet = analogRead(sensor1); // Read the analog value from sensor for wet condition
  tone(buzzerPin, 1000);
  delay(1000);
  noTone(buzzerPin);

  float rawData = dry - wet;
  correctionValue = 100.0 / rawData;

  writeToFile(sensorOneCalibration, wet, correctionValue);
  Serial.println("Calibration complete");

  readDataFromFile(sensorOneCalibration, wet, correctionValue);
}

void writeToFile(char* fileName, int wetValue, float correctionValue) {
  if (SD.exists(fileName)) {
    Serial.print("Removing ");
    Serial.println(fileName);
    SD.remove(fileName);
    Serial.println("Done");
  }

  File myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to ");
    Serial.println(fileName);
    myFile.print("WET:");
    myFile.println(wetValue); // Write the wet value to the file
    myFile.print("CORRECTION:");
    myFile.println(correctionValue, 6); // Write the correction value to the file with 6 decimal places
    myFile.close();
    Serial.println("Done");
  } else {
    Serial.print("Error opening ");
    Serial.println(fileName);
  }
}

void readDataFromFile(char* fileName, int &wetValue, float &correctionValue) {
  File myFile = SD.open(fileName);
  if (myFile) {
    while (myFile.available()) {
      String line = myFile.readStringUntil('\n');
      if (line.startsWith("WET:")) {
        wetValue = line.substring(4).toInt(); // Extract and convert wet value
      }
      if (line.startsWith("CORRECTION:")) {
        correctionValue = line.substring(11).toFloat(); // Extract and convert correction value
      }
    }
    myFile.close();
  } else {
    Serial.print("Error opening ");
    Serial.println(fileName);
  }
}
