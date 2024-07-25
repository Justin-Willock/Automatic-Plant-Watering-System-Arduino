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

int wet = 0;
int dry = 0;
int x = 0;

 void setup() {
  pinMode(water, OUTPUT);
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
    Serial.println("Put in dry soil");

  delay(1000);

  digitalWrite(water, HIGH);
  dry = analogRead(AOUT_PIN); // read the analog value from sensor for dry condition
  eraseFileContents(dryFileName);
  writeToFile(dryFileName, dry);
  Serial.println(dry);

  delay(1000);

  digitalWrite(water, LOW);  
  wet = analogRead(AOUT_PIN); // read the analog value from sensor for dry condition
  eraseFileContents(wetFileName);
  writeToFile(wetFileName, wet);
  Serial.println(wet);
  
  
  int rawData = dry-wet;
  int rawData2 = rawData;
  Serial.println(rawData);

  float correctedData = 100/rawData2;

  float trueValue = rawData*correctedData;
  Serial.println(trueValue);

  delay(5000);

  Serial.println("result from file:");
  readFromFile(wetFileName);

 }


 void loop() {

  int value = analogRead(AOUT_PIN); // read the analog value from sensor
/*  
  int hiddenValue = value-wet;
  float shownValue = hiddenValue*correctedData;


  if (shownValue > THRESHOLD) {
    digitalWrite(water, HIGH);
    Serial.print("The soil is DRY (");
  }
  else{
    digitalWrite(water, LOW);  
    Serial.print("The soil is WET (");
  }
    

  Serial.print(shownValue);
  Serial.println(")");
  delay(5000);
  */
}



//writeToFile(wetFileName, valueToWrite);  // Write to wet.txt
//writeToFile(dryFileName, valueToWrite);  // Write to dry.txt
void writeToFile(char* fileName, float valueToWrite)
{
  File myFile = SD.open(fileName, FILE_WRITE);
  if (myFile)
  {
    Serial.print("Writing to ");
    Serial.println(fileName);
    myFile.write((char*)&valueToWrite, sizeof(valueToWrite));  // Write the float value to the file
    myFile.close();
    Serial.println("Done");
  }
  else
  {
    Serial.print("Error opening ");
    Serial.println(fileName);
  }
}

//deleteFile(wetFileName);  // Delete wet.txt
//deleteFile(dryFileName);  // Delete dry.txt
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

//eraseFileContents(wetFileName);  // Erase contents of wet.txt
//eraseFileContents(dryFileName);  // Erase contents of dry.txt
void eraseFileContents(char* fileName)
{
  File myFile = SD.open(fileName, FILE_WRITE);
  if (myFile)
  {
    Serial.print("Erasing contents of ");
    Serial.println(fileName);
    myFile.close();  // Simply opening the file in write mode erases its contents
    Serial.println("Done");
  }
  else
  {
    Serial.print("Error opening ");
    Serial.println(fileName);
  }
}

//readFromFile(wetFileName);  // Read from wet.txt
//readFromFile(dryFileName);  // Read from dry.txt
void readFromFile(char* fileName)
{
  File myFile = SD.open(fileName);
  if (myFile)
  {
    Serial.print("Reading from ");
    Serial.println(fileName);

    while (myFile.available())
    {
      // Read a line from the file and print it to the serial monitor
      Serial.println(myFile.readStringUntil('\n'));

      String line = myFile.readStringUntil('\n');
      x = line.toInt();  // Convert the string to an integer
      Serial.println(x);
    }

    myFile.close();
    Serial.println("Done");
  }
  else
  {
    Serial.print("Error opening ");
    Serial.println(fileName);
  }
}

