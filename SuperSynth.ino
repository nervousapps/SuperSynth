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
//  - slide10 = A9 CC        - enbut1 = 37 SELECT                  - int3pos3 =  // CCDOWN            - input8 = 26

/* Caractères personnalisés */
byte START_DIV_0_OF_1[8] = {
  B01111, 
  B11000,
  B10000,
  B10000,
  B10000,
  B10000,
  B11000,
  B01111
}; // Char début 0 / 1

byte START_DIV_1_OF_1[8] = {
  B01111, 
  B11000,
  B10011,
  B10111,
  B10111,
  B10011,
  B11000,
  B01111
}; // Char début 1 / 1

byte DIV_0_OF_2[8] = {
  B11111, 
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111
}; // Char milieu 0 / 2

byte DIV_1_OF_2[8] = {
  B11111, 
  B00000,
  B11000,
  B11000,
  B11000,
  B11000,
  B00000,
  B11111
}; // Char milieu 1 / 2

byte DIV_2_OF_2[8] = {
  B11111, 
  B00000,
  B11011,
  B11011,
  B11011,
  B11011,
  B00000,
  B11111
}; // Char milieu 2 / 2

byte END_DIV_0_OF_1[8] = {
  B11110, 
  B00011,
  B00001,
  B00001,
  B00001,
  B00001,
  B00011,
  B11110
}; // Char fin 0 / 1

byte END_DIV_1_OF_1[8] = {
  B11110, 
  B00011,
  B11001,
  B11101,
  B11101,
  B11001,
  B00011,
  B11110
}; // Char fin 1 / 1


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
#include <Time.h>
#include <TimeAlarms.h>

AudioPlaySdWav           playSdWav1;     //xy=185,396
AudioPlaySdWav           playSdWav2;     //xy=199,472
AudioPlaySdWav           playSdWav3;     //xy=207,519
AudioPlaySdWav           playSdWav4;     //xy=219,562
AudioPlaySdWav           playSdWav5;     //xy=228,200
AudioPlaySdWav           playSdWav6;     //xy=233,333
AudioPlaySdWav           playSdWav7;     //xy=248,267
//AudioSynthNoisePink      pink1;
//AudioInputAnalog         adc1;           //xy=256,98
//AudioEffectReverb        reverb1;        //xy=411,100
AudioMixer4              mix1;         //xy=505,293
AudioMixer4              mix2;         //xy=512,420
AudioMixer4              mix3;         //xy=694,351
AudioMixer4              mix4;         //xy=694,351
AudioOutputAnalogStereo  dacs1;          //xy=847,355

// Create Audio connections between the components
AudioConnection          patchCord5(playSdWav1, 0, mix4, 1);
AudioConnection          patchCord7(playSdWav2, 0, mix1, 0);
AudioConnection          patchCord6(playSdWav3, 0, mix1, 1);
AudioConnection          patchCord1(playSdWav4, 0, mix2, 0);
AudioConnection          patchCord2(playSdWav5, 0, mix2, 1);
AudioConnection          patchCord3(playSdWav6, 0, mix2, 2);
AudioConnection          patchCord4(playSdWav7, 0, mix2, 3);
//AudioConnection          patchCord8(adc1, reverb1);
//AudioConnection          patchCord9(reverb1, 0, mix1, 0);
AudioConnection          patchCord10(mix1, 0, mix3, 0);
AudioConnection          patchCord11(mix2, 0, mix3, 1);
AudioConnection          patchCord12(mix4, 0, dacs1, 0);
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

const int NBTYPE = 9;
const int NBX = 350;
const int NBSLOT = 7;
char SAMPLE_TYPE[NBTYPE][20] = {"ALL","909", "808", "707","LINN", "DARBUKA","VOCALS", "PIANO", "OLD/VIN"};
char SAMPLE_X[NBTYPE][NBX][40];
int SLOTS[NBSLOT][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
int type = 0;
int enctype = 0;
int x = 0;
int encx = 0;
int slot = 0;
int mode = 0;
int encslot = 0;
int state = 0;
int cc = 0;

int LCD_NB_COLUMNS = 20;
unsigned int last_msec = 0;

elapsedMillis msec = 0;
elapsedMillis msec1 = 0;
elapsedMillis msec2 = 0;
elapsedMillis msec3 = 0;
int lastHour = 0;
int turn = 0;
double vol = 0;
bool modMidi = false;

// SD var
File root;
const int chipSelect = BUILTIN_SDCARD;

//************SETUP**************
void setup() {
  // Open serial communications and wait for port to open:
  //Serial.begin(9600);

  // Configure the ADCs
  analogReadResolution(9);
  analogReadAveraging(4);
  analogReference(EXTERNAL);

  // Configure the DACs
  analogWriteResolution(16);
  dacs1.analogReference(INTERNAL);
  AudioMemory(8);
  
  // loop to configure input pins and internal pullup resisters for digital section
  for (int i=0;i<D_PINS;i++){
    pinMode(DIGITAL_PINS[i], INPUT_PULLUP);
  }
  // set up the LCD's number of rows and columns:
  lcd.begin(20, 2);

  // Set mixers gain
  mix1.gain(0, 1);
  mix1.gain(1, 2);
  mix2.gain(0, 2);
  mix2.gain(1, 2);
  mix2.gain(2, 2);
  mix2.gain(3, 2);
  mix3.gain(0, 1);
  mix3.gain(1, 1);
  mix4.gain(1, 4);

  //Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    //Serial.println("initialization failed!");
    return;
  }
  //Serial.println("initialization done.");

  lcd.print("*SuperSynth UP !!!!*");

  initSAMPLE_X();

  root = SD.open("/");

  listSDfiles(root, 0);

  fillSAMPLE_X();

  delay(1000);
  setup_progressbar();

  setTime(0,0,0,14,11,93);
  printTrackLength();
}

//************LOOP**************
void loop() {
  //sendMidiNote();
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
      switch(mode){
        case 0:
          sampleplayer();
          break;

        case 1:
          sample1();
          break;

        case 2:
          sample2();
          break;
          
        case 3:
          sample3();
          break;
          
        case 4:
          sample4();
          break;

        case 5:
          sample4();
          break;

        case 6:
          sample6();
          break;

        case 7:
          sample11();
          break;
          
        case 8:
          //
          break;
          
        case 9:
          //
          break;
          
        case 10:
          //
          break;
          
        case 11:
          sample11();
          break;

         case 12:
          sample11();
          break;

        case 13:
          sample13();
          break;

        case 14:
          //
          break;

        case 15:
          sample13();
          break;

        case 16:
          //
          break;
      }
      turn = 3;
      break;

    case 3:
      getEncoderData();
      turn = 4;
      break;

    case 4:
      selectSample();
      if(msec3 >= 150 && msec3 <= 800){
        if(msec3 > last_msec + 10){
          last_msec = msec3;
          draw_progressbar(msec3/8);
        }else{
          last_msec = 0;
        }
      }
      turn = 0;
      break;
  }
  zero();
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
     i = i + 1;
     delay(5);
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

//***************ZERO SECTION******************
void zero(){
  printTrackLength();
  if(msec > 2000){
    msec = 0;
  }
  if(msec1 > 2000){
    msec1 = 0;
  }
  if(msec2 > 2000){
    msec2 = 501;
  }
  if(msec3 > 2000){
    msec3 = 801;
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

void printTrackLength(){
  lcd.setCursor(13,0);
  lcd.print(minute());
  if(hour() > lastHour){
    lastHour = hour();
    lcd.setCursor(12,0);
    lcd.print(lastHour);
  }
}

void printLCDmessCHAR(const char* mess, int col, int row, bool clr){
  if(clr){
    lcd.clear();
  }
  lcd.setCursor(col, row);
  // Print a message to the LCD.
  lcd.print(mess);
  
  lcd.setCursor(15,0);
  lcd.print("M");
  lcd.setCursor(16,0);
  if(mode == 0){
    lcd.print("D");
  }else{
    lcd.print(mode); 
  }
  
  lcd.setCursor(18,0);
  lcd.print("S");
  lcd.setCursor(19,0);
  if(slot == 0){
    lcd.print("D");
  }else{
    lcd.print(slot); 
  }
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
          if(modMidi){
            usbMIDI.sendControlChange(CCID[i], data[i], channel);
            printLCDmessCHAR("CC", 12, 1, false);
            printLCDmessINT(data[i], 16, 1);
          }else{
            vol = floor(((double)data[i]/31.75)*10)/10;
            printLCDmessCHAR("VOL", 11, 1, false);
            printLCDmessINT(i, 14, 1);
            lcd.setCursor(16,1);
            lcd.print(vol);
            switch(i){
              case 0:
                mix4.gain(1, vol);
                break;
              case 1:
                mix1.gain(0, vol);
                break;
              case 2:
                mix1.gain(1, vol);
                break;
              case 3:
                mix2.gain(0, vol);
                break;
              case 4:
                mix2.gain(1, vol);
                break;
              case 5:
                mix2.gain(2, vol);
                break;
              case 6:
                mix2.gain(3, vol);
                break;
            }
          }
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
        if(i == 17){
          if(data[i] < 125){
            modMidi = true;
          }else{
            modMidi = false;
          }
        }
        if(modMidi){
          usbMIDI.sendControlChange(CCID[i], data[i], channel);
          printLCDmessCHAR("CC", 12, 1, false);
          printLCDmessINT(data[i], 16, 1);
        }
      }
    }
  }
}


//***************** SEND MIDI NOTE ***************
//void sendMidiNote(){
//  usbMIDI.sendNoteOn(60, 99, channel);  // 60 = C4
//  printLCDmessCHAR("C4",9,0,true);
//  delay(500);
//  usbMIDI.sendNoteOn(61, 99, channel);  // 61 = C#4
//  usbMIDI.sendNoteOff(60, 0, channel);  // 60 = C4
//  printLCDmessCHAR("C#4",9,0,true);
//  delay(500);
//  usbMIDI.sendNoteOn(62, 99, channel);  // 62 = D4
//  usbMIDI.sendNoteOff(61, 0, channel);  // 61 = C#4
//  printLCDmessCHAR("D4",9,0,true);
//  delay(500);
//  usbMIDI.sendNoteOff(62, 0, channel);  // 62 = D4
//  
//}

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


//*****************909 SECTION***************
void sample1(){
  for (int i=0;i<D_PINS-2;i++){
    digital[i].update();
    if (digital[i].fallingEdge()) {
      switch (i) {
        case 0:
          playSdWav1.play("PIANO1.WAV");
          break;

        case 1:
          playSdWav2.play(SAMPLE_X[4][9]);
          break;

        case 2:
          playSdWav3.play(SAMPLE_X[6][2]);
          break;

        case 3:
          playSdWav4.play("9MT0D0.WAV");
          break;

        case 4:
          playSdWav5.play("8LTOM5.WAV");
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

//*****************ACID SECTION***************
void sample2(){
  for (int i=0;i<D_PINS-2;i++){
    digital[i].update();
    if (digital[i].fallingEdge()) {
      switch (i) {
        case 0:
          //playSdWav3.play("PIANO1.WAV");
          break;

        case 1:
          playSdWav2.play("OHHH.WAV");
          break;

        case 2:
          playSdWav1.play(SAMPLE_X[6][2]);
          break;

        case 3:
          playSdWav4.play("9MT0D0.WAV");
          break;

        case 4:
          playSdWav5.play("8LTOM5.WAV");
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

//*****************DARB SECTION***************
void sample3(){
  for (int i=0;i<D_PINS-2;i++){
    digital[i].update();
    if (digital[i].fallingEdge()) {
      switch (i) {
        case 0:
          playSdWav1.play("VYALLAH.WAV");
          break;

        case 1:
          playSdWav2.play("Lcab01t1.WAV");
          break;

        case 2:
          playSdWav3.play("Darb2.wav");
          break;

        case 3:
          playSdWav4.play("Darb3.wav");
          break;

        case 4:
          playSdWav5.play("Darb4.WAV");
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


//*****************909 SECTION***************
void sample4(){
  for (int i=0;i<D_PINS-2;i++){
    digital[i].update();
    if (digital[i].fallingEdge()) {
      switch (i) {
        case 0:
          playSdWav1.play("9ST3T0SA.WAV");
          break;

        case 1:
          playSdWav2.play(SAMPLE_X[4][9]);
          break;

        case 2:
          playSdWav3.play("VTATO.WAV");
          break;

        case 3:
          playSdWav4.play("9MT0D0.WAV");
          break;

        case 4:
          playSdWav5.play("8OHH2.WAV");
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

//*****************909 SECTION***************
void sample6(){
  for (int i=0;i<D_PINS-2;i++){
    digital[i].update();
    if (digital[i].fallingEdge()) {
      switch (i) {
        case 0:
          playSdWav1.play("OTOM5.WAV");
          break;

        case 1:
          playSdWav2.play("8CYM7.WAV");
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

//*****************SPLASH SECTION***************
void sample11(){
  for (int i=0;i<D_PINS-2;i++){
    digital[i].update();
    if (digital[i].fallingEdge()) {
      switch (i) {
        case 0:
          playSdWav1.play("9ST3T0SA.WAV");
          break;

        case 1:
          playSdWav2.play("8OHH1.WAV");
          break;

        case 2:
          playSdWav3.play(SAMPLE_X[6][13]);
          break;

        case 3:
          playSdWav4.play(SAMPLE_X[4][34]);
          break;

        case 4:
          playSdWav5.play(SAMPLE_X[4][35]);
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


//*****************SEX SECTION***************
void sample13(){
  for (int i=0;i<D_PINS-2;i++){
    digital[i].update();
    if (digital[i].fallingEdge()) {
      switch (i) {
        case 0:
          playSdWav1.play(SAMPLE_X[6][6]);
          break;

        case 1:
          playSdWav2.play("9HHOD2.WAV");
          break;

        case 2:
          playSdWav3.play(SAMPLE_X[6][0]);
          break;

        case 3:
          playSdWav4.play("9RIDED0.WAV");
          break;

        case 4:
          playSdWav5.play("9RIDED2.WAV");
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
        enctype = newRightF;
        if(enctype > NBTYPE - 1){
          knobRightF.write(0);
          enctype = 0;
        }
        if(enctype < 0){
          knobRightF.write((NBTYPE-1)*2);
          enctype = NBTYPE-1;
        }
        printLCDmessCHAR(SAMPLE_TYPE[enctype], 0, 1, true);
        break;

      case 1:
        encx = newRightF;
        for(int i = 0; i<NBX; i++){
          if(SAMPLE_X[type][i][0] == 0){
            maxx = i;
            break;
          }
        }
        if(encx < 0){
          knobRightF.write(maxx*2);
          encx = maxx;
        }
        if(encx > maxx){
          knobRightF.write(0);
          encx = 0;
        }
        printLCDmessCHAR(SAMPLE_X[type][encx], 0, 1, true);
        printLCDmessINT(encx, 0, 0);
        break;
    }
    positionRightF = newRightF;
  }
  if (newRightS != positionRightS){
    if (newRightS > 17){
      knobRightS.write(0);
      newRightS = 0;
    }
    if (newRightS < 0){
      newRightS = 16;
      knobRightS.write(newRightS*2);
    }
    if (newRightS < 17){
      encslot = newRightS;
      printLCDmessCHAR("SLOT/MODE", 0, 0, true);
      if(encslot == 0){
        printLCDmessCHAR("D", 10, 0, false);
      }else{
        printLCDmessINT(encslot, 10, 0);
      }
    }
    positionRightS = newRightS;
  }
}
//************** SELECT PROGRAM SECTION *****************
void selectSample(){
  if(digital[7].update()){
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
              knobRightF.write(type*2);
              enctype = type;
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
  }
  if(digital[8].update()){
    if (digital[8].fallingEdge()) {
        msec3 = 0;
        lcd.setCursor(0,0);
        lcd.print(" ");
    }
    if (digital[8].risingEdge()){
      if(msec3 >= 800) {
        mode = encslot;
        printLCDmessCHAR("MODE SET !", 0, 0, true);
      }else{
        if (encslot > NBSLOT - 1){
          printLCDmessCHAR("ONLY 7 SLOTS", 0, 0, true);
          msec3 = 801;
        }else{
          slot = encslot;
          printLCDmessCHAR("SLOT SET !", 0, 0, true);
          msec3 = 801;
        }
      }
    }
  }
}

/**
 * Fonction de configuration de l'écran LCD pour la barre de progression.
 * Utilise les caractères personnalisés de 0 à 6 (7 reste disponible).
 */
void setup_progressbar() {

  /* Enregistre les caractères personnalisés dans la mémoire de l'écran LCD */
  lcd.createChar(0, START_DIV_0_OF_1);
  lcd.createChar(1, START_DIV_1_OF_1);
  lcd.createChar(2, DIV_0_OF_2);
  lcd.createChar(3, DIV_1_OF_2);
  lcd.createChar(4, DIV_2_OF_2);
  lcd.createChar(5, END_DIV_0_OF_1);
  lcd.createChar(6, END_DIV_1_OF_1);
}

/**
 * Fonction dessinant la barre de progression.
 *
 * @param percent Le pourcentage à afficher.
 */
void draw_progressbar(byte percent) {
 /* Déplace le curseur sur la seconde ligne */
  lcd.setCursor(0, 0);
 
  /* Map la plage (0 ~ 100) vers la plage (0 ~ LCD_NB_COLUMNS * 2 - 2) */
  byte nb_columns = map(percent, 0, 100, 0, LCD_NB_COLUMNS * 2 - 2);
  // Chaque caractère affiche 2 barres verticales, mais le premier et dernier caractère n'en affiche qu'une.

  /* Dessine chaque caractère de la ligne */
  for (byte i = 0; i < LCD_NB_COLUMNS; ++i) {

    if (i == 0) { // Premiére case

      /* Affiche le char de début en fonction du nombre de colonnes */
      if (nb_columns > 0) {
        lcd.write(1); // Char début 1 / 1
        nb_columns -= 1;

      } else {
        lcd.write((byte) 0); // Char début 0 / 1
      }

    } else if (i == LCD_NB_COLUMNS -1) { // Derniére case

      /* Affiche le char de fin en fonction du nombre de colonnes */
      if (nb_columns > 0) {
        lcd.write(6); // Char fin 1 / 1

      } else {
        lcd.write(5); // Char fin 0 / 1
      }

    } else { // Autres cases

      /* Affiche le char adéquat en fonction du nombre de colonnes */
      if (nb_columns >= 2) {
        lcd.write(4); // Char div 2 / 2
        nb_columns -= 2;

      } else if (nb_columns == 1) {
        lcd.write(3); // Char div 1 / 2
        nb_columns -= 1;

      } else {
        lcd.write(2); // Char div 0 / 2
      }
    }
  }
}
