/*
 Fil: Audio_Recorder.ino
 Skrivare: Edward Fröberg
 Datum: 2023-12-09
 Komponenter: Arduino UNO, Large microphone module, SD card module, 3x button
 Beskrivning: Detta program spelar in ljud med en mikrofon och sparar ljudet som en fil.
              Mikrofnens sample rate kan ändras vid behov. Allt kontrolleras med olika knappar.
 */

#include <TMRpcm.h>
#include <SD.h>
#include <SPI.h>

TMRpcm audio;
int file_number = 0;
char filePrefixname[50] = "audio";
char exten[10] = ".wav";
const int recordLed = 2;
const int mic_pin = A0;
int sample_rate = 5000; // Sample rate från början

#define SD_CSPin 4

const int recordButton = 3;    // Knapp som kontrollerar inspelning
const int sampleRateUpButton = 5;  // Knapp som ökar Sample rate
const int sampleRateDownButton = 6; // Knapp som sänker Sample rate

bool isRecording = false;
char currentFileName[50] = ""; // Sparar nuvarande filnamn

void setup() {
  Serial.begin(9600);
  pinMode(mic_pin, INPUT);
  pinMode(recordLed, OUTPUT);
  pinMode(recordButton, INPUT_PULLUP);
  pinMode(sampleRateUpButton, INPUT_PULLUP);
  pinMode(sampleRateDownButton, INPUT_PULLUP);

  Serial.println("Ready for recording");

  if (!SD.begin(SD_CSPin)) {
    Serial.println("An error occurred while mounting SD"); // Meddelande vid eventuellt fel gällande SD kortet
  }

  audio.CSPin = SD_CSPin;
}


/* 
  Reglerar knapparna och deras funktioner samt kontrollerar inspelningen
*/

void loop() {
  if (digitalRead(recordButton) == LOW && !isRecording) {
    // Startar inspelningen när knappen är nedtryckt
    startRecording();
    isRecording = true;
  } else if (digitalRead(recordButton) == HIGH && isRecording) {
    // Stannar inspelningen när knappen släpps
    stopRecording();
    isRecording = false;
    file_number++;
  }

  // Ökar Sample rate när knappen är nedtryckt
  if (digitalRead(sampleRateUpButton) == LOW) {
    increaseSampleRate();
    delay(250); // Undviker för hastiga knapptryck
  }

  // Sänker Sample rate när knappen är nedtryckt
  if (digitalRead(sampleRateDownButton) == LOW) {
    decreaseSampleRate();
    delay(250); // Undviker för hastiga knapptryck
  }
}


/*
  Funktion för att generera filer och starta inspelning av ljud från mikrofonen
 */

void startRecording() {
  Serial.println("####################################################################################"); // Visar när man är på en ny fil
  char fileSlNum[20] = "";
  itoa(file_number, fileSlNum, 10);
  char file_name[50] = "";
  strcat(file_name, filePrefixname);
  strcat(file_name, fileSlNum);
  strcat(file_name, exten);
  strcpy(currentFileName, file_name); // Sparar nuvarande filnamn
  Serial.print("New File Name: ");
  Serial.println(currentFileName);
  digitalWrite(recordLed, HIGH);
  audio.startRecording(currentFileName, sample_rate, mic_pin);
  Serial.println("startRecording ");
}


/*
 Funktion för att stanna nuvarande inspelning
 */
void stopRecording() {
  digitalWrite(recordLed, LOW);
  audio.stopRecording(currentFileName); // Stannar nuvarande inspelning
  Serial.println("stopRecording");
}


/*
 Funktion för att öka Sample rate
 */
void increaseSampleRate() {
  sample_rate += 1000; // Ökar Sample rate med 1000
  if (sample_rate > 31000) {
    sample_rate = 31000; // Anger en maximal Sample rate
  }
  Serial.print("Increased sample rate to: ");
  Serial.println(sample_rate);
}


/*
 Funktion för att sänka Sample rate
 */
void decreaseSampleRate() {
  sample_rate -= 1000; // Sänker Sample rate med 1000
  if (sample_rate < 1000) {
    sample_rate = 1000; // Anger en minimal Sample rate
  }
  Serial.print("Decreased sample rate to: ");
  Serial.println(sample_rate);
}
