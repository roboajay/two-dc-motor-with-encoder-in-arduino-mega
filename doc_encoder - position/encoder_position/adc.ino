
ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  TCNT1 = timer1_counter;   // preload timer
   senso=analogRead(A0);
   senso = pow(3027.4 / senso, 1.2134);

  
 }

