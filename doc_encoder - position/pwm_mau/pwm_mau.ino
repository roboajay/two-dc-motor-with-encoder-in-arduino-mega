void setup() {
 PORTD=0X00;
 DDRD=(1<<5)|(1<<6);
 TCCR0A=0B10100011;
 TCCR0B=0B10100011;
 TCNT0=0;
 ///
 // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR0A=128;
OCR0B=64;
 }

void loop() {
  // put your main code here, to run repeatedly:

}
ISR(TIMER1_COMPA_vect)     {  
  
}

