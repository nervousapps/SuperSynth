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

#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>


AudioPlaySdWav           playSdWav4;     //xy=185,396
AudioPlaySdWav           playSdWav5;     //xy=199,472
AudioPlaySdWav           playSdWav6;     //xy=207,519
AudioPlaySdWav           playSdWav7;     //xy=219,562
AudioPlaySdWav           playSdWav1;     //xy=228,200
AudioPlaySdWav           playSdWav3;     //xy=233,333
AudioPlaySdWav           playSdWav2;     //xy=248,267
//AudioInputAnalog         adc1;           //xy=256,98
AudioEffectReverb        reverb1;        //xy=411,100
AudioMixer4              mix1;         //xy=505,293
AudioMixer4              mix2;         //xy=512,420
AudioMixer4              mix3;         //xy=694,351
AudioOutputAnalogStereo  dacs1;          //xy=847,355

// Create Audio connections between the components
AudioConnection          patchCord1(playSdWav4, 0, mix2, 0);
AudioConnection          patchCord2(playSdWav5, 0, mix2, 1);
AudioConnection          patchCord3(playSdWav6, 0, mix2, 2);
AudioConnection          patchCord4(playSdWav7, 0, mix2, 3);
AudioConnection          patchCord5(playSdWav1, 0, mix1, 1);
AudioConnection          patchCord6(playSdWav3, 0, mix1, 3);
AudioConnection          patchCord7(playSdWav2, 0, mix1, 2);
//AudioConnection          patchCord8(adc1, reverb1);
AudioConnection          patchCord9(reverb1, 0, mix1, 0);
AudioConnection          patchCord10(mix1, 0, mix3, 0);
AudioConnection          patchCord11(mix2, 0, mix3, 1);
AudioConnection          patchCord12(mix3, 0, dacs1, 0);
AudioConnection          patchCord13(mix3, 0, dacs1, 1);


// ******CONSTANT VALUES********
// customize code behaviour here!
const int channel = 1; // MIDI channel
const int A_PINS = 18; // number of Analog PINS
const int D_PINS = 9; // number of Digital PINS
const int ON_VELOCITY = 99; // note-one velocity sent from buttons (should be 65 to  127)

// define the pins you want to use and the CC ID numbers on which to send them..
const int ANALOG_PINS[A_PINS] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17};
const int CCID[A_PINS] = {1,2,3,4,5,6,7,9,10,11,12,13,14,15,16,17,18};

// define the pins and notes for digital events
const int DIGITAL_PINS[D_PINS] = {25,26,24,29,30,27,28,37,10};
const int BOUNCE_TIME = 5; // 5 ms is usually sufficient
const boolean toggled = true;


//******VARIABLES***********
// a data array and a lagged copy to tell when MIDI changes are required
byte data[A_PINS];
byte dataLag[A_PINS]; // when lag and new are not the same then update MIDI CC value

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
  {ANALOG_PINS[15],true},
  {ANALOG_PINS[16],true},
  {ANALOG_PINS[17],true}
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

const int NBTYPE = 4;
const int NBX = 150;
const int NBSLOT = 7;
char SAMPLE_TYPE[NBTYPE][20] = {"ALL","909","707","LINN"};
char SAMPLE_X[NBTYPE][NBX][40];
int SLOTS[NBSLOT][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
int type = 0;
int enctype = 0;
int x = 0;
int encx = 0;
int slot = 0;
int encslot = 0;
int state = 0;
int cc = 0;

elapsedMillis msec = 0;
elapsedMillis msec1 = 0;
elapsedMillis msec2 = 0;
int turn = 0;

// SD var
File root;
const int chipSelect = BUILTIN_SDCARD;

//************SETUP**************
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  analogReadResolution(9);
  analogReadAveraging(4);
  analogReference(EXTERNAL);
  dacs1.analogReference(INTERNAL);
  AudioMemory(8);
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

  Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  initSAMPLE_X();

  root = SD.open("/");

  listSDfiles(root, 0);

  fillSAMPLE_X();

  lcd.print("*SuperSynth UP !!!!*");

  delay(3000);
}

//************LOOP**************
void loop() {
  switch(turn){
    case 0:
      if (msec >= 50) {
        msec = 0;
        midiControllerSliders();
      }
      turn = 1;
      break;

    case 1:
      if (msec1 >= 50) {
        msec1 = 0;
        midiControllerPotBut();
      }
      turn = 2;
      break;

    case 2:
      sampleplayer();
      turn = 3;
      break;

    case 3:
      getEncoderData();
      turn = 4;
      break;

    case 4:
      selectSample();
      turn = 0;
      break;
  }
  while (usbMIDI.read()) {
     // controllers must call .read() to keep the queue clear even if they are not responding to MIDI
  }
}


//***************List SD files****************
void listSDfiles(File dir, int numTabs){
  int i = 0;
  while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.println(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       //printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       strcpy(SAMPLE_X[0][i], entry.name());
     }
     entry.close();
     i++;
   }
}

void initSAMPLE_X(){
  for(int i = 0; i<NBTYPE; i++){
    for(int j = 0; j<NBX; j++){
      SAMPLE_X[i][j][0] = 0;
    }
  }
}

void fillSAMPLE_X(){
  for(int i = 0; i<NBX; i++){
    if(SAMPLE_X[0][i][0] == 0){
      break;
    }
    for(int j = 0; j<NBTYPE; j++){
      if(SAMPLE_X[0][i][0] == SAMPLE_TYPE[j][0]){
        for(int k = 0; k<NBX; k++){
          if(SAMPLE_X[j][k][0] == 0){
            strcat(SAMPLE_X[j][k],SAMPLE_X[0][i]);
            break;
          }
        }
      }
    }
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

void printLCDmessCHAR(const char* mess, int col, int row, bool clr){
  if(clr){
    lcd.clear();
  }
  lcd.setCursor(col, row);
  // Print a message to the LCD.
  lcd.print(mess);
}

//************MIDI CONTROLLER SECTION**************
void midiControllerSliders(){
  for (int i=0;i<A_PINS - 8;i++){
    // update the ResponsiveAnalogRead object every loop
    analog[i].update();
    if (analog[i].hasChanged()) {
      data[i] = analog[i].getValue()>>2;
      if (data[i] > dataLag[i]+2 || data[i] < dataLag[i]-2) {
        if(data[i] >= 115){
          data[i] = 127;
        }
        if(data[i] <= 10){
          data[i] = 0;
        }
        if (data[i] != dataLag[i]){
          dataLag[i] = data[i];
          usbMIDI.sendControlChange(CCID[i], data[i], channel);
          printLCDmessCHAR("CC", 12, 1, false);
          printLCDmessINT(data[i], 16, 1);
          Serial.print("CC ");
          Serial.println(data[i]);
        }
      }
    }
  }
}

void midiControllerPotBut(){
  for (int i=10;i<A_PINS;i++){
    // update the ResponsiveAnalogRead object every loop
    analog[i].update();
    if (analog[i].hasChanged()) {
      data[i] = analog[i].getValue()>>2;
      if (data[i] >= dataLag[i] || data[i] <= dataLag[i]){
        dataLag[i] = data[i];
        usbMIDI.sendControlChange(CCID[i], data[i], channel);
        printLCDmessCHAR("CC", 12, 1, false);
        printLCDmessINT(data[i], 16, 1);
        Serial.print("CC ");
        Serial.println(data[i]);
      }
    }
  }
}

//***************** SAMPLE PLAYER SECTION ***************
void sampleplayer(){
  for (int i=0;i<D_PINS-2;i++){
    digital[i].update();
    if (digital[i].fallingEdge()) {
      switch (i) {
        case 0:
          playSdWav1.play(SAMPLE_X[SLOTS[i][0]][SLOTS[i][1]]);
          break;

        case 1:
          playSdWav2.play(SAMPLE_X[SLOTS[i][0]][SLOTS[i][1]]);
          break;

        case 2:
          playSdWav3.play(SAMPLE_X[SLOTS[i][0]][SLOTS[i][1]]);
          break;

        case 3:
          playSdWav4.play(SAMPLE_X[SLOTS[i][0]][SLOTS[i][1]]);
          break;

        case 4:
          playSdWav5.play(SAMPLE_X[SLOTS[i][0]][SLOTS[i][1]]);
          break;

        case 5:
          playSdWav6.play(SAMPLE_X[SLOTS[i][0]][SLOTS[i][1]]);
          break;

        case 6:
          playSdWav7.play(SAMPLE_X[SLOTS[i][0]][SLOTS[i][1]]);
          break;

      }
    }
  }
}

//*****************ENCODER SECTION***************
void getEncoderData(){
  int maxx = 0;
  long newRightF, newRightS;
  newRightF = knobRightF.read()/2;
  newRightS = knobRightS.read()/2;
  if (newRightF != positionRightF) {
    switch(state){
      case 0:
        if(newRightF > NBTYPE - 1){
          knobRightF.write(0);
          newRightF = 0;
        }
        if(newRightF < 0){
          knobRightF.write(NBTYPE+2);
          newRightF = NBTYPE+2;
        }
        enctype = newRightF;
        printLCDmessCHAR(SAMPLE_TYPE[enctype], 0, 1, true);
        break;

      case 1:
        for(int i = 0; i<NBX; i++){
          if(SAMPLE_X[type][i][0] == 0){
            maxx = i;
            break;
          }
        }
        if(SAMPLE_X[type][newRightF][0] == 0){
          knobRightF.write(0);
          newRightF = 0;
        }
        if(newRightF < 0){
          newRightF = maxx;
          knobRightF.write(maxx);
        }
        encx = newRightF;
        printLCDmessCHAR(SAMPLE_X[type][encx], 0, 1, true);
        break;
    }
    positionRightF = newRightF;
  }
  if (newRightS != positionRightS){
    if (newRightS > NBSLOT - 1){
      knobRightS.write(0);
      newRightS = 0;
    }
    if (newRightS < 0){
      newRightS = NBSLOT+6;
      knobRightS.write(newRightS);
    }
    if (newRightS < NBSLOT){
      encslot = newRightS;
      printLCDmessCHAR("SLOT ", 0, 0, true);
      printLCDmessINT(encslot, 6, 0);
    }
    positionRightS = newRightS;
  }
}
//************** SELECT PROGRAM SECTION *****************
void selectSample(){
  digital[7].update();
  digital[8].update();
  if (digital[7].fallingEdge()) {
      switch(state){
        case 0:
          type = enctype;
          printLCDmessCHAR(SAMPLE_X[type][0], 0, 1, true);
          knobRightF.write(0);
          msec2 = 0;
          state = 1;
          break;
        
        case 1:
          if(msec2 <= 500){
            printLCDmessCHAR(SAMPLE_TYPE[type], 0, 1, true);
            knobRightF.write(type);
            state = 0;
          }else{
            x = encx;
            SLOTS[slot][0] = type;
            SLOTS[slot][1] = x;
            state = 1;
          }
          msec2 = 0;
          break;
      }
  }
  if (digital[8].fallingEdge()) {
      slot = encslot;
      printLCDmessCHAR("SLOT SET !", 0, 0, true);
  }
}
