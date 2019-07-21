// CONTROLLER
//  - slide1 = A0 CC         - pot1 = VOLUME                       - enbut2 = 10 SELECT               - bp1 = RESET             - input9 = 27
//  - slide2 = A1 CC         - pot2 = CONTRASTE                    - int1pos1 = A15 CCUP              - bp2 = PROGRAM           - input10 = 28
//  - slide3 = A2 CC         - pot3 = A10 CC                       - int1pos2 =  // CCNO              - input1 = AUDIOOUT        
//  - slide4 = A3 CC         - pot4 = A11 CC                       - int1pos3 =  // CCDOWN .          - input2 = AUDIOOUT
//  - slide5 = A4 CC         - pot5 = A12 CC                       - int2pos1 = A16 CCUP              - input3 = 
//  - slide6 = A5 CC         - pot6 = A13 CC                       - int2pos2 =  // CCNOT             - input4 = 29
//  - slide7 = A6 CC         - pot7 = A14 CC                       - int2pos3 =  // CCDOWN            - input5 = 30
//  - slide8 = A7 CC         - enc1 = 6,7 SELECT                   - int3pos1 = A17 CCUP              - input6 = 24
//  - slide9 = A8 CC         - enc2 = 8,9 SELECT                   - int3pos2 =  // CCNOT             - input7 = 25
//  - slide10 = A9 CC        - enbut1 = 38 SELECT                  - int3pos3 =  // CCDOWN            - input8 = 26


//************LIBRARIES USED**************
// include the ResponsiveAnalogRead library for analog smoothing
#include <ResponsiveAnalogRead.h>
// include the Bounce library for 'de-bouncing' switches -- removing electrical chatter as contacts settle
#include <Bounce.h>
//usbMIDI.h library is added automatically when code is compiled as a MIDI device
#include <LiquidCrystalFast.h>
// This optional setting causes Encoder to use more optimized code,
// It must be defined before Encoder.h is included.
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#include <Audio.h>
#include <Wire.h>

// WAV files converted to code by wav2sketch
#include "AudioSampleCowbell.h"
#include "AudioSampleCrash.h"
#include "AudioSampleRim.h"
#include "AudioSampleSnare.h"
#include "AudioSampleTom1.h"
#include "AudioSampleTom2.h"
#include "AudioSampleTom3.h"
#include "AudioSampleRide_909.h"

const unsigned int* SAMPLENAMES[] = {AudioSampleCowbell,AudioSampleCrash,AudioSampleRim,AudioSampleSnare,AudioSampleTom1,AudioSampleTom2,AudioSampleTom3,AudioSampleRide_909};
char SAMPLEMAP[11][40] = {"Cowbell","Crash","Rim","Snare","Tom1","Tom2","Tom3","Ride_909","8","9","10"};

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioPlayMemory    sound0;
AudioPlayMemory    sound1;  // four memory players, so we can play
AudioPlayMemory    sound2;  // four sounds simultaneously
AudioPlayMemory    sound3;
AudioPlayMemory    sound4;
AudioPlayMemory    sound5;  // four memory players, so we can play
AudioPlayMemory    sound6;  // four sounds simultaneously
AudioPlayMemory    sound7;
AudioMixer4        mix1;    // one 4-channel mixer
AudioMixer4        mix2;    // one 4-channel mixer
AudioMixer4        mix3;    // one 4-channel mixer
AudioOutputAnalogStereo  dac;     // play to both I2S audio board and on-chip DAC

//Effects
//AudioEffectReverb        reverb1;
//AudioEffectChorus        chorus1;
//AudioEffectFlange        flange1;
//AudioEffectFreeverb      freeverb1;
//AudioEffectGranular      granular1;
//AudioEffectEnvelope      envelope1;
//AudioEffectBitcrusher    bitcrusher1;
char EFFECTS[7][40] = {"Reverb","Chorus","Flanger","Freeverb","Granular","Envelope","Bitcrusher"};

// Create Audio connections between the components
//
AudioConnection c1(sound0, 0, mix1, 0);
AudioConnection c2(sound1, 0, mix1, 1);
AudioConnection c3(sound2, 0, mix1, 2);
AudioConnection c4(sound3, 0, mix1, 3);
AudioConnection c5(sound4, 0, mix2, 0);
AudioConnection c6(sound5, 0, mix2, 1);
AudioConnection c7(sound6, 0, mix2, 2);
AudioConnection c8(sound7, 0, mix2, 3);
AudioConnection c9(mix1, 0, mix3, 0);
AudioConnection c10(mix2, 0, mix3, 1);
AudioConnection c11(mix3, 0, dac, 0);
AudioConnection c12(mix3, 0, dac, 1);


// ******CONSTANT VALUES********
// customize code behaviour here!
const int channel = 1; // MIDI channel
const int A_PINS = 18; // number of Analog PINS
const int D_PINS = 9; // number of Digital PINS
const int ON_VELOCITY = 99; // note-one velocity sent from buttons (should be 65 to  127)

// define the pins you want to use and the CC ID numbers on which to send them..
const int ANALOG_PINS[A_PINS] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17};
const int CCID[A_PINS] = {24,25,26,27,28,31,64,102,103,104,105,106,107,108,109,110,120,121};

// define the pins and notes for digital events
const int DIGITAL_PINS[D_PINS] = {24,25,26,27,28,29,30,38,10};
const int BOUNCE_TIME = 5; // 5 ms is usually sufficient
const boolean toggled = true;


//******VARIABLES***********
// a data array and a lagged copy to tell when MIDI changes are required
int previous[A_PINS] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,-1,-1,-1,-1,-1};//,-1,-1};


//************INITIALIZE LIBRARY OBJECTS**************
// not sure if there is a better way... some way run a setup loop on global array??
// use comment tags to comment out unused portions of array definitions

// initialize the ReponsiveAnalogRead objects
ResponsiveAnalogRead analog[]{
  {ANALOG_PINS[0],true},
  {ANALOG_PINS[1],true},
  {ANALOG_PINS[2],true},
  {ANALOG_PINS[3],true},
  {ANALOG_PINS[4],true},
  {ANALOG_PINS[5],true},
  {ANALOG_PINS[6],true},
  {ANALOG_PINS[7],true},
  {ANALOG_PINS[8],true},
  {ANALOG_PINS[9],true},
  {ANALOG_PINS[10],true},
  {ANALOG_PINS[11],true},
  {ANALOG_PINS[12],true},
  {ANALOG_PINS[13],true},
  {ANALOG_PINS[14],true},
  {ANALOG_PINS[15],true}
};

// initialize the bounce objects
Bounce digital[] =   {
  Bounce(DIGITAL_PINS[0],BOUNCE_TIME),
  Bounce(DIGITAL_PINS[1], BOUNCE_TIME),
  Bounce(DIGITAL_PINS[2], BOUNCE_TIME),
  Bounce(DIGITAL_PINS[3], BOUNCE_TIME),
  Bounce(DIGITAL_PINS[4], BOUNCE_TIME),
  Bounce(DIGITAL_PINS[5], BOUNCE_TIME),
  Bounce(DIGITAL_PINS[6], BOUNCE_TIME),
  Bounce(DIGITAL_PINS[7], BOUNCE_TIME),
  Bounce(DIGITAL_PINS[8], BOUNCE_TIME)
};


// initialize the library with the numbers of the interface pins
// LiquidCrystal lcd(RS, RW, Enable, D4, D5, D6, D7)
LiquidCrystalFast lcd(12, 39, 11, 5, 4, 3, 2);

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder knobRightF(6, 7);
Encoder knobRightS(8, 9);
long positionRightF = -999;
long positionRightS = -999;

int sample = 0;
int encsample = 0;
int effect = 0;
int enceffect;

//************SETUP**************
void setup() {
  analogReadResolution(7);
  dac.analogReference(INTERNAL);
  AudioMemory(24);
  // loop to configure input pins and internal pullup resisters for digital section
  for (int i=0;i<D_PINS;i++){
    pinMode(DIGITAL_PINS[i], INPUT_PULLUP);
  }
  // set up the LCD's number of rows and columns:
  lcd.begin(20, 2);

  mix1.gain(0, 1);
  mix1.gain(1, 1);
  mix1.gain(2, 1);
  mix1.gain(3, 1);
  mix2.gain(0, 1);
  mix2.gain(1, 1);
  mix2.gain(2, 1);
  mix2.gain(3, 1);

  lcd.print("**** SuperSynth UP !!! ****");

  delay(3000);
}

elapsedMillis msec = 0;
elapsedMillis msec0 = 0;
//************LOOP**************
void loop() {
  getEncoderData();
  selectSample();
  if (msec >= 40) {
    msec = 0;
    midiControllerSliders();
  }
  if (msec0 >= 20) {
    msec0 = 0;
    midiControllerPotBut();
  }
  sampleplayer();
  while (usbMIDI.read()) {
     // controllers must call .read() to keep the queue clear even if they are not responding to MIDI
  }
}

//***************SCREEN SECTION****************
void printLCDmessINT(int mess, int col, int row){
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(col, row);
  // Print a message to the LCD.
  lcd.print(mess);
}

void printLCDmessCHAR(const char* mess, int col, int row){
  lcd.clear();
  lcd.setCursor(col, row);
  // Print a message to the LCD.
  lcd.print(mess);
}

//************MIDI CONTROLLER SECTION**************
void midiControllerSliders(){
  for (int i=0;i<A_PINS-8;i++){
    // update the ResponsiveAnalogRead object every loop
    int n = analogRead(ANALOG_PINS[i]);
    if(n > 115){
      n = 127;
    }
    if(n < 10){
      n = 0;
    }
    if (n < (previous[i] - 3) || n > (previous[i] + 3)) {
      previous[i] = n;
      printLCDmessCHAR("MIDI CC", 0, 0);
      printLCDmessINT(n, 0, 1);
      usbMIDI.sendControlChange(CCID[i], n, channel);
    }
  }
}

void midiControllerPotBut(){
  for (int i=10;i<A_PINS;i++){
    // update the ResponsiveAnalogRead object every loop
    int n = analogRead(ANALOG_PINS[i]);
    if (n < (previous[i] - 1) || n > (previous[i] + 1)) {
      printLCDmessCHAR("MIDI CC", 0, 0);
      printLCDmessINT(n, 0, 1);
      usbMIDI.sendControlChange(CCID[i], n, channel);
      previous[i] = n;
    }
  }
}

//***************** SAMPLE EFFECT PLAYER ****************
void sampleeffectplayer(){
  sound0.play(SAMPLENAMES[sample]);
  printLCDmessCHAR(SAMPLEMAP[sample], 0, 0);
}

//***************** SAMPLE PLAYER SECTION ***************
void sampleplayer(){
  for (int i=0;i<D_PINS-2;i++){
    digital[i].update();
    if (digital[i].fallingEdge()) {
      switch (i) {
        case 0:
          sound2.play(SAMPLENAMES[i]);
          printLCDmessCHAR(SAMPLEMAP[i], 0, 0);
          break;

        case 1:
          sound1.play(SAMPLENAMES[i]);
          printLCDmessCHAR(SAMPLEMAP[i], 0, 0);
          break;

        case 2:
          sampleeffectplayer();
          break;

        case 3:
          sound3.play(SAMPLENAMES[i]);
          printLCDmessCHAR(SAMPLEMAP[i], 0, 0);
          break;

        case 4:
          sound4.play(SAMPLENAMES[i]);
          printLCDmessCHAR(SAMPLEMAP[i], 0, 0);
          break;

        case 5:
          sound5.play(SAMPLENAMES[i]);
          printLCDmessCHAR(SAMPLEMAP[i], 0, 0);
          break;

        case 6:
          sound6.play(SAMPLENAMES[i]);
          printLCDmessCHAR(SAMPLEMAP[i], 0, 0);
          break;

      }
    }
  }
}

//*****************ENCODER SECTION***************
void getEncoderData(){
  long newRightF, newRightS;
  newRightF = knobRightF.read()/2;
  newRightS = knobRightS.read()/2;
  if (newRightF != positionRightF) {
    if (newRightF < 11 && newRightF > 0){
      encsample = newRightF;
      printLCDmessCHAR(SAMPLEMAP[encsample], 0, 1);
    }
    positionRightF = newRightF;
  }if (newRightS != positionRightS){
    if (newRightS < 7 && newRightS > 0){
      enceffect = newRightS;
      printLCDmessCHAR(EFFECTS[enceffect], 0, 1);
    }
    positionRightS = newRightS;
  }
}
//************** SELECT PROGRAM SECTION *****************
void selectSample(){
  digital[7].update();
  digital[8].update();
  if (digital[7].fallingEdge()) {
      sample = encsample;
      printLCDmessCHAR("OK SAMPLE", 0, 1); //SAMPLEMAP[sample]
  }
  if (digital[8].fallingEdge()) {
      effect = enceffect;
      printLCDmessCHAR("OK EFFECT", 0, 1); //EFFECTS[effect]
  }
}
