#define AOUT_PIN A0 // Arduino pin that connects to AOUT pin of moisture sensor
#define THRESHOLD 500 // CHANGE YOUR THRESHOLD HERE
#include <SD.h>
#include <SPI.h>

File wetFile;  // Declare wetFile and dryFile variables
char wetFileName[] = "wet.txt";
File dryFile;
char dryFileName[] = "dry.txt";
const int chipSelect = 10;
char charRead;

const int water = 8;

int currentValue = 0;
int wet = 0;
int dry = 0;
int x = 0;

const int buzzer = 6; //buzzer to arduino pin 9
const int buttonPin = 4;
int buttonState = 0;  // variable for reading the pushbutton status

void calibrate();
void writeToFile(char* fileName, int valueToWrite);
void deleteFile(char* fileName);
void eraseFileContents(char* fileName);
void readFromFile(char* fileName);

float correctedData = 0;

void setup() {
  pinMode(water, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  Serial.begin(9600);
  Serial.println("Simple SD Card Demo");

  if (SD.begin(chipSelect))
  {
    Serial.println("SD card is present & ready");
  } 
  else
  {
    Serial.println("SD card missing or failure");
    while(1);  //wait here forever
  }
  

  readFromFile(wetFileName);
  wet = x;
  Serial.print("WET VALUE:");
  Serial.println(wet);
  readFromFile(dryFileName);
  dry = x;
  Serial.print("DRY VALUE:");
  Serial.println(dry);




  float rawData = dry - wet;
  correctedData = 100/rawData;
  Serial.print("correction value: ");
  Serial.println(correctedData);


  readFromFile(wetFileName);
  wet = x;
  readFromFile(dryFileName);
  dry = x;
  


  Serial.println("result from file:");
  readFromFile(wetFileName);
  Serial.println("direct x result:");
  Serial.println(x);
}


void loop() {
  buttonState = digitalRead(buttonPin);
  int value = analogRead(AOUT_PIN); // read the analog value from sensor

  if (buttonState == HIGH) {
    calibrate();
  }




  currentValue = analogRead(AOUT_PIN);

  int Data = currentValue - wet;
  int realTimeValue = 100-(Data * correctedData);
  Serial.print("HERE:::");
  Serial.println(realTimeValue);



  digitalWrite(water, HIGH);
  delay(1000);
  digitalWrite(water, LOW); 
  /*
  if (shownValue > THRESHOLD) {
    digitalWrite(water, HIGH);
    Serial.print("The soil is DRY (");
  }
  else {
    digitalWrite(water, LOW);  
    Serial.print("The soil is WET (");
  }
    
  Serial.print(shownValue);
  Serial.println(")");
  */
}

void calibrate(){
  Serial.println("Put in dry soil");
  
  tone(buzzer, 1000);
  delay(1000);
  noTone(buzzer);

  delay(100000);

  digitalWrite(water, HIGH);
  dry = analogRead(AOUT_PIN); // read the analog value from sensor for dry condition
  deleteFile("dry.txt");
  writeToFile(dryFileName, dry);
  Serial.println(dry);

  tone(buzzer, 1000);
  delay(1000);
  noTone(buzzer);

  delay(100000);

  digitalWrite(water, LOW);  
  wet = analogRead(AOUT_PIN); // read the analog value from sensor for wet condition
  deleteFile("wet.txt");
  writeToFile(wetFileName, wet);
  Serial.println(wet);

  int rawData = dry - wet;
  correctedData = 100 / rawData;

  readFromFile(wetFileName);
  wet = x;
  readFromFile(dryFileName);
  dry = x;
}

void writeToFile(char* fileName, int valueToWrite)
{
  File myFile = SD.open(fileName, FILE_WRITE);
  if (myFile)
  {
    Serial.print("Writing to ");
    Serial.println(fileName);
    myFile.println(valueToWrite); // Write the integer value to the file
    myFile.close();
    Serial.println("Done");
  }
  else
  {
    Serial.print("Error opening ");
    Serial.println(fileName);
  }
}

void deleteFile(char* fileName)
{
  if (SD.exists(fileName))
  {
    Serial.print("Removing ");
    Serial.println(fileName);
    SD.remove(fileName);
    Serial.println("Done");
  }
  else
  {
    Serial.print("File not found: ");
    Serial.println(fileName);
  }
}

void readFromFile(char* fileName)
{
  File myFile = SD.open(fileName);
  if (myFile)
  {
    //Serial.print("Reading from ");
    //Serial.println(fileName);

    while (myFile.available())
    {
      String line = myFile.readStringUntil('\n');
      if (line.length() > 0)
      {
        x = line.toInt();  // Convert the string to an integer
        //Serial.println(x);
      }
    }

    myFile.close();
    //Serial.println("Done");
  }
  else
  {
    Serial.print("Error opening ");
    Serial.println(fileName);
  }
}

