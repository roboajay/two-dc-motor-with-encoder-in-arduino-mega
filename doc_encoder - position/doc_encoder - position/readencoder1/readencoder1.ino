int encoder0Count;
int deg = 0;
int encoder0PinA = 2;
int encoder0PinB = 3;

int encoder0CountChanged = 0;

void setup(){
  Serial.begin(115200);
  Serial.println("hello");
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);  
  
  attachInterrupt(0, doEncoder0A, CHANGE);
  attachInterrupt(1, doEncoder0B, CHANGE);

}//setup

void loop(){
  if (encoder0CountChanged != 0) {
    encoder0CountChanged = 0;
    Serial.println(encoder0Count);
  }
}

void doEncoder0A(){
  if( digitalRead(encoder0PinB) == 0 ) {
    if ( digitalRead(encoder0PinA) == 0 ) {
      // A fell, B is low
      encoder0Count--; // moving reverse
    } else {
      // A rose, B is low
      encoder0Count++; // moving forward
    }
 } else {
    if ( digitalRead(encoder0PinA) == 0 ) {
      // A fell, B is high
      encoder0Count++; // moving forward
    } else {
      // A rose, B is high
      encoder0Count--; // moving reverse
    }
  }
  /* Make sure the encoder0Count are between 0 and 359
  if (encoder0Count > 359) {
    encoder0Count = encoder0Count - 360;
  } else if (encoder0Count < 0) {
    encoder0Count = encoder0Count + 360;
  }
 */
  // tell the loop that the encoder0Count have changed
  encoder0CountChanged = 1;
}

void doEncoder0B(){
  if ( digitalRead(encoder0PinA) == 0 ) {
    if ( digitalRead(encoder0PinB) == 0 ) {
      // B fell, A is low
      encoder0Count++; // moving forward
    } else {
      // B rose, A is low
      encoder0Count--; // moving reverse
    }
 } else {
    if ( digitalRead(encoder0PinB) == 0 ) {
      // B fell, A is high
      encoder0Count--; // moving reverse
    } else {
      // B rose, A is high
      encoder0Count++; // moving forward
    }
  }
 /* // Make sure the encoder0Count are between 0 and 359
  if (encoder0Count > 359) {
    encoder0Count = encoder0Count - 360;
  } else if (encoder0Count < 0) {
    encoder0Count = encoder0Count + 360;
  }
*/
  // tell the loop that the encoder0Count have changed
  encoder0CountChanged = 1;
}

