/*
    KontinuumLAB Code Class - Ocarina sketch for the Teensy Microcontroller
    Copyright (C) 2021 Jeppe Rasmussen - KontinuumLAB
    www.kontinuumlab.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/



int breathPin = 22;

int key[4] = {18, 19, 20, 21};

int pressedKeys[4];

int keyPhase;
int correct;
int newNote;

int currentNote;
int lastNote;

int breath;
int lastBreath;

const int fingeringArray[14][8] = {
  {100, 0, 1, 2, 3, 100, 60}, // C
  {3, 100, 0, 1, 2, 100, 59}, // B
  {1, 100, 0, 2, 3, 100, 58}, // Bb
  {0, 100, 1, 2, 3, 100, 57}, // A
  {0, 3, 100, 1, 2, 100, 56}, // Ab
  {0, 2, 100, 1, 3, 100, 55}, // G
  {0, 2, 3, 100, 1, 100, 54}, // F#
  {0, 1, 100, 2, 3, 100, 53}, // F
  {0, 1, 3, 100, 2, 100, 52}, // E
  {1, 2, 100, 0, 3, 100, 51}, // D#
  {0, 1, 2, 100, 3, 100, 50}, // D
  {1, 2, 3, 100, 0, 100, 49}, // C#
  {0, 1, 2, 3, 100, 100, 48}  // C  
};

void setup() {
  pinMode(key[0], INPUT_PULLUP);
  pinMode(key[1], INPUT_PULLUP);
  pinMode(key[2], INPUT_PULLUP);
  pinMode(key[3], INPUT_PULLUP);
  pinMode(breathPin, INPUT_PULLUP);
  
}


void loop() {
  int i;
  lastBreath = breath;
  breath = !digitalRead(breathPin);
  
  if(breath != 0){ 
    for (i = 0; i < 4; i++){
      pressedKeys[i] = !digitalRead(key[i]);
    }

    //-------------KEY ANALISYS-------------------//
  
    // Variables for the fingering-analysis section:
    correct = 0;
    newNote = 0;
    lastNote = currentNote;


    // Main fingering-analysis for-loop.
    // "f" = position within "f"ingering array, "n" = position within the "n"ote array:
    int f;
    int n;
    for(f = 0; f < 13; f++){
      // During first keyPhase, check if the requested sensor is active. 
      keyPhase = 0;
      for(n = 0; n < 7; n++){
        if(fingeringArray[f][n] != 100){

          // If active sensors coincide with "fingerings" array requirements: "correct" = yes.
          // Else: "correct" = no and exit current "fingering" array.
          
          // During first keyPhase, check:
          if(keyPhase == 0){  // Check "closed" keys
            if(pressedKeys[fingeringArray[f][n]] == 1){
              correct = 1;
            }
            else{
              correct = 0;
              break;
            }
          }
          // During second keyPhase, check 
          else if(keyPhase == 1){ // Check "open" keys
            if(pressedKeys[fingeringArray[f][n]] == 0){
              correct = 1;
            }
            else{
              correct = 0;
              break;
            }
          }       
        }
        // When a "100" is encountered, add 1 to keyPhase, except if keyPhase is 1, exit loop:
        else if(fingeringArray[f][n] == 100){
          if(keyPhase == 1){
            break;
          }
          else{
            keyPhase ++;
          }
        }
      }
      // if "correct" is still yes after all loops finish, then a "fingerings" array has perfectly coincided with
      // current situation. Set "currentNote to the note defined at the end of that array:
      if(correct == 1){
        currentNote = fingeringArray[f][6];
        break;
      }
    }
    // If correct is 1, then there is a new note to report, so set "newNote" to 1, and set "currentNote" to
    // currentNote plus octave: 
    if(correct == 1){
      if(currentNote != lastNote){
        newNote = 1;
      }
    }
    // If newNote is a yes, then send MIDI messages. lastNote off and currentNote On
    if(newNote == 1){
      usbMIDI.sendNoteOn(lastNote, 0, 1);
      usbMIDI.sendNoteOn(currentNote, 127, 1);
    }
    // If breathsensor was just activated then also send MIDI message noteOn for currentNote
    else if(breath != 0 && lastBreath == 0){
      usbMIDI.sendNoteOn(currentNote, 127, 1);
    }
  }
  if(breath == 0 && lastBreath != 0){
    usbMIDI.sendNoteOn(currentNote, 0, 1);
  }
}



