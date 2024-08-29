#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include  "arduinoFFT.h"
 
#define SAMPLES 128             //SAMPLES-pt FFT. Must  be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2048  //Ts = Based on Nyquist, must be 2 times the highest expected frequency.

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

arduinoFFT  FFT = arduinoFFT();
 
unsigned int samplingPeriod;
unsigned long microSeconds;
  
double vReal[SAMPLES];
double vImag[SAMPLES];

String instruments[] = {"Violin", "Guitar"};
int instrumentNotes[] = {4, 6};
char noteLabels[2][6] = { {'G', 'D', 'A', 'E'}, {'E', 'A', 'D', 'G', 'B', 'E'} };

double noteFrequencies[2][6] = { {196, 293, 440, 659, 0, 0}, {82.41, 110, 146.83, 196, 246.94, 329.63} };

int numInstruments = 2;
int selectedInstrument = 0;
int selectedNote = 0;

bool listening = false;
bool buttonPressed = false;
 
void setup() {
    Serial.begin(115200); //Baud rate for the Serial Monitor
    samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY)); //Period in microseconds  

    lcd.begin(16, 2);
    lcd.setBacklight(WHITE);
}



void updateDisplay() {
    if(listening) {
        lcd.setCursor(0, 0);
        
        //lcd.print("<display stuff>");

    // TODO: Implement "progress bar" display and color changing here
    
  
      if (selectedInstrument ==0){
        switch(selectedNote){
          case 0:
            lcd.print("G note");
            break;
          case 1:
            lcd.print("D note");
            break;
          case 2:
            lcd.print("A note");
            break;
          case 3:
            lcd.print("E note");
             break;
         }}
     
      if(selectedInstrument == 1){
    
       switch(selectedNote){
        case 0:
          lcd.print("E note");
          break;
        case 1:
          lcd.print("A note");
           break;
        case 2:
          lcd.print("D note");
          break;
       case 3:
          lcd.print("G note");
          break;
       case 4:
          lcd.print("B note");
          break;
       case 5:
          lcd.print("E note");
          break; 
         }}
 
    double targetFrequency = noteFrequencies[selectedInstrument][selectedNote];
    double detectedFrequency = FFT.MajorPeak(vReal,  SAMPLES, SAMPLING_FREQUENCY);
    while ( detectedFrequency != targetFrequency){

        for(int  i=0; i<SAMPLES; i++) {
        microSeconds = micros();
        vReal[i] = analogRead(0);
        vImag[i] = 0;
        // delay for sample period
        while(micros() < (microSeconds + samplingPeriod)) {

        }
      FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.Compute(vReal,  vImag, SAMPLES, FFT_FORWARD);
      FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
      lcd.setCursor(0,1);
      double detectedFrequency = FFT.MajorPeak(vReal,  SAMPLES, SAMPLING_FREQUENCY);
      double current = targetFrequency-detectedFrequency;
      lcd.setCursor(0, 1);
      lcd.print(current);
      delay(1000);
    }




  }}

    
  else {
    lcd.setCursor(0, 0);
    lcd.print(instruments[selectedInstrument]);
  
    lcd.setCursor(0, 1);
    for(int i = 0; i < 6; i++) {
      if(i < instrumentNotes[selectedInstrument]) {
        if(i == selectedNote) {
          lcd.print(">");
        } else {
          lcd.print(" ");
        }
        lcd.print(noteLabels[selectedInstrument][i]);
      } else {
        lcd.print("  ");
      }
    }
  }
}
 
void loop() {
    uint8_t buttons = lcd.readButtons();
    if (buttonPressed == false && buttons) {
      lcd.clear();
      buttonPressed = true;
      if (buttons & BUTTON_UP) {
        //lcd.setBacklight(RED);
        selectedInstrument++;
        if(selectedInstrument >= numInstruments) selectedInstrument = 0;
        selectedNote = 0;
        listening = false;
      }
      if (buttons & BUTTON_DOWN) {
        selectedInstrument--;
        if(selectedInstrument < 0) selectedInstrument = numInstruments - 1;
        selectedNote = 0;
        listening = false;
      }
      if (buttons & BUTTON_LEFT) {
        selectedNote--;
        if(selectedNote < 0) selectedNote = instrumentNotes[selectedInstrument] - 1;
        listening = false;
      }
      if (buttons & BUTTON_RIGHT) {
        selectedNote++;
        if(selectedNote >= instrumentNotes[selectedInstrument]) selectedNote = 0;
        listening = false;
      }
      
    if ((buttons & BUTTON_SELECT) && (selectedInstrument == 0)) {
      switch (selectedNote) {
        case 0:  // your hand is on the sensor
        lcd.print("G note");
        lcd.setCursor(0, 1);
        lcd.print("196HZ");
        //updateDisplay();
        listening = !listening;
        break;
      
      case 1:  // your hand is close to the sensor
      lcd.print("D note");
      listening = !listening;
        break;
   
      case 2:  // your hand is a few inches from the sensor
      lcd.print("A note");
      listening = !listening;
      break;
   
      case 3:  // your hand is nowhere near the sensor
      lcd.print("E note");
      listening = !listening;
      break;
        }
          delay(1);  // delay in between reads for stability
      }
    
  if ((buttons & BUTTON_SELECT) && (selectedInstrument == 1)) {
    switch (selectedNote) {
     case 0:
       // your hand is on the sensor
      lcd.print("E1 note");
      lcd.setCursor(0, 1);
       listening = !listening;
      break;
      

      case 1:  // your hand is close to the sensor
      lcd.clear();
     lcd.print("A note");
     listening = !listening;
      break;
    

      case 2:  // your hand is a few inches from the sensor
      lcd.print("D note");
      listening = !listening;
      break;

     case 3:  // your hand is nowhere near the sensor
      lcd.print("G note");
      listening = !listening;
      break;

         case 4:  // your hand is nowhere near the sensor
      lcd.print("B note");
      listening = !listening;
      break;
        
        case 5 :  // your hand is nowhere near the sensor
      lcd.print("E note");
      listening = !listening;
      break;
    }
    delay(1);  // delay in between reads for stability
      }}

     else { if(buttons == 0) buttonPressed = false;}
    
      updateDisplay();
      }