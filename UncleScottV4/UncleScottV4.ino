//That’s what a bug is, an annoying "aunt"!

#include <SD.h> // sd card library
#include <SPI.h> // hookups for sd card
#include "RTC.h" // clock for datalogging feature

const int sensor[] = {A0, A1, A2, A3, A4}; // Arduino pin connected to the AOUT pin of the moisture sensor

const int chipSelect = 10; // sd card 
const int waterSolenoid = 8; // solenoid on pin 8
const int buzzerPin = 6; // Buzzer to Arduino pin 6
const int buttonPin = 4; // calibrate button

char sensorCalibration[] = "sensorCalibration.txt"; // file setup for all calibration data
char runningData[] = "runningData.txt"; // file setup for all operating data
const int arraySize = 5; // Adjust this to match the number of floats written it will adjust the number of sensors that can be used
float readArray[arraySize]; // the temparary variable used to read the array off the sd card

int sensorDisconnected = 0; //needs actual value will provide a threshold to discount these sensors from calculations 

int currentValue = 0; // this is used for the raw data straight off the sensor
int wet[arraySize] = {0}; // this is used to store the value the sensors have when in fully wet soil
int dry[arraySize] = {0}; // this is used to store the value the sensors have when in fully dry soil
float correctionValue[arraySize] = {0.0}; // this value is used to complete the equations for calibrating all sensors
int buttonState = 0; // Variable for reading the pushbutton status
int desiredWaterContent = 50; // will be set by user in the future allowing for diffrent plant types currently set for tomatoes

void setup() {
  pinMode(waterSolenoid, OUTPUT); // sets this pin to be an output
  pinMode(buzzerPin, OUTPUT); // sets this pin to be an output
  pinMode(buttonPin, INPUT); // sets this pin to be an input

  Serial.begin(9600); // starts communication with computer

  readFromFile(sensorCalibration, wet, dry, correctionValue); //reads calibration data from sd card on startup

  
}

void loop() {
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH) { // if the button is pressed the calibrate function is called.
    calibrate();
  }

  Serial.println(analogRead(sensor[0]));

  for (int i = 0; i < arraySize; i++){
    if(analogRead(sensor[i]) < wet[i]){
      wet[i] = analogRead(sensor[i]);
      writeToFile(sensorCalibration, wet, dry, correctionValue);
    }
  }


  int sum = 0;
  for (int i = 0; i < arraySize; i++) {
    if (analogRead(correctionValue[i]) != 0) {// disregards disconnected sensors
      int value[arraySize] = {0};
      int y[arraySize] = {0};
      int valueCorrected[arraySize] = {0};
      
      value[i] = analogRead(sensor[i]);
      y[i] = value[i] - wet[i];
      valueCorrected[i] = 100 - (y[i] * correctionValue[i]);
      sum += valueCorrected[i];
    }
  }
  int average = (float)sum / arraySize;

  if (desiredWaterContent > average) {
    digitalWrite(waterSolenoid, HIGH); // turn on water
    delay(5000); // wait 5 seconds
    digitalWrite(waterSolenoid, LOW); // turn off water
    delay(10000); // wait 10 seconds
  }


}

void calibrate() { // calibrates all sensors at the same time
  Serial.println("Put all in DRY soil"); // commands user to put all sensors into dry soil for first mesurement
  tone(buzzerPin, 1000); // alerts user
  delay(1000); // for one second
  noTone(buzzerPin); //then stops alerting user
  delay(120000); // waits 120 seconds to allow for full sensor stability 

  for (int i = 0; i < arraySize; i++) {
    dry[i] = analogRead(sensor[i]); // sets this dry array position equal to it's asocciated sensor's value
  }

  for (int i = 0; i < arraySize; i++) { // this compares the sensor diconnected value to the value given then deactivateds that sensor if it is in fact disconected 
    if (dry[i] == sensorDisconnected) { // this compares the sensor diconnected value to the value 
      dry[i] = 2; // disactivates sensor if the sensor is disconnected
    }
  }

  Serial.println("Put all in WET soil"); // commands user to put all sensors into wet soil for first mesurement
  tone(buzzerPin, 1000); // alerts user
  delay(1000); // for one second
  noTone(buzzerPin); //then stops alerting user
  delay(120000); // waits 120 seconds to allow for full sensor stability 

  for (int i = 0; i < arraySize; i++) {
    wet[i] = analogRead(sensor[i]); // sets this dry array position equal to it's asocciated sensor's value
  }

  for (int i = 0; i < arraySize; i++) { // this compares the sensor diconnected value to the value given then deactivateds that sensor if it is in fact disconected 
    if (wet[i] == sensorDisconnected) { // this compares the sensor diconnected value to the value
      wet[i] = 1; // disactivates sensor if the sensor is disconnected
    }
  }

  for (int i = 0; i < arraySize; i++) { // steps through the 5 diffrent sensors and independantly preforms and saves the calculations for each 
    float x[arraySize] = {0};
    x[i] = dry[i] - wet[i]; // uses a temporary x value to store the diffrence between min and max
    correctionValue[i] = 100.0 / x[i]; // uses the temporary x variable to form the correction value for each sensor
  }

  for (int i = 0; i < arraySize; i++) { // this compares the sensor diconnected value to the value given then deactivateds that sensor if it is in fact disconected 
    if (correctionValue[i] == 100.0) { // this compares the sensor diconnected value to the value
      correctionValue[i] = 0; // disactivates sensor if the sensor is disconnected
    }
  }
  writeToFile(sensorCalibration, wet, dry, correctionValue);

}

void writeArrayToFile(File &file, const char* label, int index, float value) {
  file.print(label); // starts the label for the array, e.g., "wet", "dry", or "correctionValue"
  file.print("["); // starts the index formatting, e.g., "wet[1]"
  file.print(index); // sets the correct index value, e.g., "wet[1]"
  file.print("]: "); // ends the index formatting, e.g., "wet[1]: "
  file.println(value); // writes the actual value, e.g., "wet[1]: 23"
}

void writeToFile(char* fileName, int wet[], int dry[], float correctionValue[]) { 
  // write all data to the SD card
  if (strcmp(fileName, sensorCalibration) == 0) { 
    // if the file is this type it will delete it because that's easier than erasing file contents
    if (SD.exists(sensorCalibration)) { 
      Serial.print("Removing ");
      Serial.println(sensorCalibration);
      SD.remove(sensorCalibration);
      Serial.println("Done");

    } else {
      Serial.print("Error opening ");
      Serial.println(sensorCalibration);
    }

    File myFile = SD.open(sensorCalibration, FILE_WRITE);
    if (myFile) {
      Serial.print("Writing to ");
      Serial.println(sensorCalibration);

      for (int i = 0; i < arraySize; i++) { 
        // repeat instructions multiple times
        writeArrayToFile(myFile, "wet", i, wet[i]); // saves wet array
        writeArrayToFile(myFile, "dry", i, dry[i]); // saves dry array
        writeArrayToFile(myFile, "correctionValue", i, correctionValue[i]); // saves correctionValue array
      }
      myFile.close();
      Serial.println("Done");

    } else {
      Serial.print("Error opening ");
      Serial.println(sensorCalibration);
    }
  }

  if (strcmp(fileName, runningData) == 0) {
    // Add relevant code here
  }
}

void readFromFile(char* fileName, int wet[], int dry[], float correctionValue[]) {
  if (strcmp(fileName, sensorCalibration) == 0) {
    File myFile = SD.open(sensorCalibration, FILE_READ);
    if (myFile) {
      Serial.print("Reading from ");
      Serial.println(sensorCalibration);

      int i = 0; // Index for arrays
      while (myFile.available()) {
        String line = myFile.readStringUntil('\n'); // Read a line from the file

        if (line.startsWith("wet[")) {
          int startIndex = line.indexOf('[') + 1;
          int endIndex = line.indexOf(']');
          int index = line.substring(startIndex, endIndex).toInt();
          wet[index] = line.substring(line.indexOf(": ") + 2).toInt();
        } else if (line.startsWith("dry[")) {
          int startIndex = line.indexOf('[') + 1;
          int endIndex = line.indexOf(']');
          int index = line.substring(startIndex, endIndex).toInt();
          dry[index] = line.substring(line.indexOf(": ") + 2).toInt();
        } else if (line.startsWith("correctionValue[")) {
          int startIndex = line.indexOf('[') + 1;
          int endIndex = line.indexOf(']');
          int index = line.substring(startIndex, endIndex).toInt();
          correctionValue[index] = line.substring(line.indexOf(": ") + 2).toFloat();
        }

        i++;
      }
      
      myFile.close();
      Serial.println("Done");
    } else {
      Serial.print("Error opening ");
      Serial.println(sensorCalibration);
    }
  }
}