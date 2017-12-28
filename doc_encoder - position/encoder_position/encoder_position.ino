
// ++++++++++++for encoder++++++++++++++++++++++++
#define encoder0PinA 2
#define encoder0PinB 3
int a;
int timer1_counter;
volatile float senso = 0;// bien tro
volatile long int encoder0Count = 0;// dem xung lock
volatile float angle0 = 0;// goc dat duoc
volatile float velocity0 = 0;// toc do dat duoc
volatile float angle0_previous = 0;// goc truoc
volatile float angle0_post = 0;// goc cuoi
////++++++++++++++++++++for timer2++++++++++++++++++++++++++
volatile int tcnt2 = 131;
volatile int t = 0;
// ++++++++++++++++++++for PWM lm298++++++++++++++++++++++
int enA = 6;
int in1 = 9;
int in2 = 8;
 // ++++++++++for PID control system+++++++++++++++++++
 volatile long int Ctrl_Angle=0,
                           err=0,
                           pre_err=0;
 volatile float    Kp=1.2,//1.2
                   Ki=0,
                   Kd=3.921;//3.921
 volatile long int P=0,
                   I=0,
                   D=0,
                   PID=0; 
 
 //++++++++++++++++++++++++++++++++++++++++++++++++++++++
void setup() {
  // +++++++++++++++++++++++++timer0 for pwm 1ms++++++++++++++++
                     PORTD=0X00;
                     DDRD=(1<<5)|(1<<6);
                     TCCR0A=0B10100011;
                     TCCR0B=0B10100011;
                     TCNT0=0;
                     OCR0A=0;
                     OCR0B=0;
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // ++++++++++++++++++for encoder 0+++++++++++++++++++++++++++++++
                     pinMode(encoder0PinA, INPUT_PULLUP); 
                     pinMode(encoder0PinB, INPUT_PULLUP); 
                     attachInterrupt(0, doEncoder0A, CHANGE);  
                     attachInterrupt(1, doEncoder0B, CHANGE);  
  // for PWM lm 298
    pinMode(enA, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
     pinMode(13, OUTPUT);
// setup for Timer2: interrupts every 1 ms
  TIMSK2 &= ~(1<<TOIE2);
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~(1<<WGM22);
  ASSR &= ~(1<<AS2);
  TIMSK2 &= ~(1<<OCIE2A);
  TCCR2B |= (1<<CS22)  | (1<<CS20);
  TCCR2B &= ~(1<<CS21);
  TCNT2 = tcnt2;
  TIMSK2 |= (1<<TOIE2);
  
  Serial.begin (19200); // for debugging
  //// timer1  for adc 1ms
  TCCR1A = 0;
  TCCR1B = 0;
  // Set timer1_counter to the correct value for our interrupt interval
  timer1_counter = 65474;
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
}

void loop() {
  
      // for encoder 0
  Serial.print("cnt0: ");
  Serial.print(encoder0Count, DEC); 
  Serial.print("; ");
  Serial.print("agl0: ");
  Serial.print(angle0, 4); 
  Serial.print("; ");
  Serial.print("vel0: ");
  Serial.print(velocity0, 4); 
  Serial.print("; "); 
 Serial.println(err);  
  Serial.println(senso); 
 
}
