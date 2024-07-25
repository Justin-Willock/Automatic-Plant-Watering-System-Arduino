#include <SD.h> // sd card library
#include <SPI.h> // hookups for sd card

#define sensor1 A0 // Arduino pin connected to the AOUT pin of the moisture sensor
#define sensor2 A1 // Arduino pin connected to the AOUT pin of the moisture sensor
#define sensor3 A2 // Arduino pin connected to the AOUT pin of the moisture sensor
#define sensor4 A3 // Arduino pin connected to the AOUT pin of the moisture sensor
#define sensor5 A4 // Arduino pin connected to the AOUT pin of the moisture sensor

const int chipSelect = 10; // sd card 
const int waterSolenoid = 8; // solenoid on pin 8
const int buzzerPin = 6; // Buzzer to Arduino pin 6
const int buttonPin = 4; // calibrate button


char sensorCalibration[] = "sensorCalibration.txt"; // file setup for all calibration data
char runningData[] = "runningData.txt"; // file setup for all operating data
const int arraySize = 5; // Adjust this to match the number of floats written it will adjust the number of sensors that can be used
float readArray[arraySize]; // the temparary variable used to read the array off the sd card

int sensorDisconnected = 0 //needs actual value will provide a threshold to discount these sensors from calculations 

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
}

void loop() {
  // put your main code here, to run repeatedly:

}

void calibrate() { // calibrates all sensors at the same time
  serial.println("Put all in DRY soil") // commands user to put all sensors into dry soil for first mesurement
  tone(buzzerPin, 1000); // alerts user
  delay(1000); // for one second
  noTone(buzzerPin); //then stops alerting user
  delay(120000); // waits 120 seconds to allow for full sensor stability 

  dry[0] = analogRead(sensor1); // sets this dry array position equal to it's asocciated sensor's value
  dry[1] = analogRead(sensor2); // sets this dry array position equal to it's asocciated sensor's value
  dry[2] = analogRead(sensor3); // sets this dry array position equal to it's asocciated sensor's value
  dry[3] = analogRead(sensor4); // sets this dry array position equal to it's asocciated sensor's value
  dry[4] = analogRead(sensor5); // sets this dry array position equal to it's asocciated sensor's value

  for (int i = 0; i < arraySize; i++) { // this compares the sensor diconnected value to the value given then deactivateds that sensor if it is in fact disconected 
    if (dry[i]) == sensorDisconnected) { // this compares the sensor diconnected value to the value 
      dry[i] = 2 // disactivates sensor if the sensor is disconnected
    }
  }

  serial.println("Put all in WET soil") // commands user to put all sensors into wet soil for first mesurement
  tone(buzzerPin, 1000); // alerts user
  delay(1000); // for one second
  noTone(buzzerPin); //then stops alerting user
  delay(120000); // waits 120 seconds to allow for full sensor stability 

  wet[0] = analogRead(sensor1); // sets this wet array position equal to it's asocciated sensor's value
  wet[1] = analogRead(sensor2); // sets this wet array position equal to it's asocciated sensor's value
  wet[2] = analogRead(sensor3); // sets this wet array position equal to it's asocciated sensor's value
  wet[3] = analogRead(sensor4); // sets this wet array position equal to it's asocciated sensor's value
  wet[4] = analogRead(sensor5); // sets this wet array position equal to it's asocciated sensor's value

  for (int i = 0; i < arraySize; i++) { // this compares the sensor diconnected value to the value given then deactivateds that sensor if it is in fact disconected 
    if (wet[i]) == sensorDisconnected) { // this compares the sensor diconnected value to the value
      wet[i] = 1 // disactivates sensor if the sensor is disconnected
    }
  }

  for (int i = 0; i < arraySize; i++) { // steps through the 5 diffrent sensors and independantly preforms and saves the calculations for each 
    float x[i] = dry[i] - wet[i]; // uses a temporary x value to store the diffrence between min and max
    correctionValue[i] = 100.0 / x[i]; // uses the temporary x variable to form the correction value for each sensor
  }

  for (int i = 0; i < arraySize; i++) { // this compares the sensor diconnected value to the value given then deactivateds that sensor if it is in fact disconected 
    if (correctionValue[i]) == 100.0) { // this compares the sensor diconnected value to the value
      correctionValue[i] = 0 // disactivates sensor if the sensor is disconnected
    }
  }

}

void writeToFile(char* fileName, int wet[], int dry[], float correctionValue[]) {
  if (fileName == sensorCalibration) {
    if (SD.exists(sensorCalibration)) {
      Serial.print("Removing ");
      Serial.println(sensorCalibration);
      SD.remove(sensorCalibration);
      Serial.println("Done");
    }

  File myFile = SD.open(sensorCalibration, FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to ");
    Serial.println(sensorCalibration);
    for (int i = 0; i < arraySize; i++){ // repete instructions multiple times
      myFile.print("wet["); // starts to format for "wet[1]: 23" saves wet array
      myFile.print(i); // sets correct i value in "wet[1]: 23" saves wet array
      myFile.print("]: "); // ends started section "wet[1]: 23" saves wet array
      myFile.print(wet[i]); // ends with the variable vlaue in "wet[1]: 23" saves wet array
      myFile.print("dry["); // starts to format for "wet[1]: 23" saves wet array
      myFile.print(i); // sets correct i value in "wet[1]: 23" saves dry array
      myFile.print("]: "); // ends started section "wet[1]: 23" saves dry array
      myFile.print(dry[i]); // ends with the variable vlaue in "wet[1]: 23" saves dry array
    }
    
    myFile.print("wet[]:");
    myFile.println(wet[]); // Write the wet value to the file
    myFile.print("dry[]:");
    myFile.println(dry[]); 
    myFile.print("correctionValue[]:");
    myFile.println(correctionValue[], 6); // Write the correction value to the file with 6 decimal places
    myFile.close();
    Serial.println("Done");
  } else {
    Serial.print("Error opening ");
    Serial.println(sensorCalibration);
  }
  }
  if (fileName == runningData)

  


}