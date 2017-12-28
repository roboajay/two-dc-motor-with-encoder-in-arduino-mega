//--------------------
// for encoder 0
//--------------------
void doEncoder0A(){  // interrupt 0 function
  if (digitalRead(encoder0PinA) == HIGH) {  // look for a low-to-high on channel A
    if (digitalRead(encoder0PinB) == LOW) { // check channel B to see which way encoder is turning
      encoder0Count = encoder0Count + 1;
    } 
    else {
      encoder0Count = encoder0Count - 1;
    }
  }
  else {                                     // must be a high-to-low edge on channel A
    if (digitalRead(encoder0PinB) == HIGH) { // check channel B to see which way encoder is turning  
      encoder0Count = encoder0Count + 1;
    } 
    else {
      encoder0Count = encoder0Count - 1;
    }
  }
  angle0 = 0.156*encoder0Count; 
    //angle0=round(angle0);
}

void doEncoder0B(){  // interrupt 1 function
  if (digitalRead(encoder0PinB) == HIGH) {   // look for a low-to-high on channel B
    if (digitalRead(encoder0PinA) == HIGH) { // check channel A to see which way encoder is turning
      encoder0Count = encoder0Count + 1;
    } 
    else {
      encoder0Count = encoder0Count - 1;
    }
  }
  else {                                    // must be a high-to-low edge on on channel B
    if (digitalRead(encoder0PinA) == LOW) { // check channel B to see which way encoder is turning
      encoder0Count = encoder0Count + 1;
    } 
    else {
      encoder0Count = encoder0Count - 1;
    }
  }
  angle0 = 0.156*encoder0Count; 
   //  angle0=round(angle0);
}
ISR(TIMER2_OVF_vect) {  
  TCNT2 = tcnt2;  // reload the timer
  t++;
  
  if (t == 1){
    angle0_previous = angle0; 
    
  }
  else if (t == 20){
    angle0_post = angle0;  
  }
  else if (t == 21){
    velocity0 = (angle0_post - angle0_previous)*50;  // unit: rad/s
    t = 0;
  
  }
    Motor_PID(Ctrl_Angle);
  
}
