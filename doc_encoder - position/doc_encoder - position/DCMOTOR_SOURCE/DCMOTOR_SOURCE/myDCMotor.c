//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// Author : Ho Thanh Tam	                                                         ;
// Email:   thanhtam.h@gmail.com													 ;		
// Date :   2009, Sep., 15                                                           ;
// Version: 0.1                                                                      ;
// Title:   myDCMotor                                                                ;
// website: www.hocavr.com                                                           ;
// Description: Dieu khien van toc dong co DC bang giai thuat PID                    ;                                ;
//                                                                                   ; 
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdio.h>
#include <myLCD.h>

#ifndef cbi 
	#define cbi(port, bit) 	   (port) &= ~(1 << (bit))
#endif
#ifndef sbi 
	#define sbi(port, bit) 	   (port) |=  (1 << (bit))
#endif

//Dinh nghia cac duong dieu khien motor
#define MOTOR_DDR   DDRD
#define MOTOR_PORT  PORTD
#define MOTOR_DIR   6   
#define MOTOR_EN    7   

#define Sampling_time		25	 //thoi gian lay mau (ms)
#define inv_Sampling_time	40 	 // 1/Sampling_time 
#define PWM_Period			8000 //8000 cycles = 1ms, f=8MHz

volatile	long int Pulse,  pre_Pulse;
volatile	long int rSpeed, Err, pre_Err, Kp=8, Kd=10, Ki=1; //for speed control
volatile	long int pPart=0, iPart=0, dPart=0; //PID gains
volatile 	long int Ctrl_Speed=5;	 //van toc can dieu khien (desired speed)
volatile    long int Output;
volatile 	unsigned char sample_count=0;

//dieu khien van toc bang PID
void Motor_Speed_PID(long int des_Speed){
	rSpeed=Pulse-pre_Pulse;     //tinh van toc (trong sampling time)
	pre_Pulse=Pulse;            //luu lai gia tri Pulse: so xung 
	Err=des_Speed-rSpeed;  //tinh error (loi)
    //cac thanh phan cua PID
	pPart=Kp*Err;
	dPart=Kd*(Err-pre_Err)*inv_Sampling_time;	
	iPart+=Ki*Sampling_time*Err/1000;
	Output +=pPart+dPart+iPart;     //cong thuc duoc bien doi vi la dieu khien van toc
    //saturation
	if (Output >=PWM_Period) Output=PWM_Period-1;
	if (Output <=0) Output=1;
    
    OCR1A=Output; //gan duty cycle cho OCR1A: update PWM    
	pre_Err=Err;  //luu lai gia tri error  
}


int main(void){
    unsigned char dis[5]; //bien tam, hien thi LCD
	Pulse=0;
	//Encoder va cac chan nhap toc do	
	DDRB=0x00; //set PORTB as a input port to use the T0 input pin and INT2
	PORTB=0xFF; //dien tro keo len (nhat la encoder)       
	//Motor
    MOTOR_DDR=0xF0;
    sbi(MOTOR_PORT, MOTOR_DIR);
    
	//ngat ngoai, cho encoder--------------------------------------------------------------
	MCUCSR|=(0<<ISC2); //ngat INT2 la ngat canh xuong - Falling Edge
	GICR  |=(1<<INT2);    //Enable INT2
	//---------------------------------------------------------------------------------

	//dung timer 2 lam bo dinh thi 25ms, sampling time--------------------------------
	TCCR2=(1<<CS22)|(1<<CS21)|(1<<CS20); //CS22=1, CS21=1, CS20=1: chon Prescaler=1024 (trang 125 datasheet)
	TCNT2=60;           //gan gia tri khoi tao cho T/C2 de duoc 25ms (truong hop f=8MHz)
    TIMSK=(1<<TOIE2);  // cho phep ngat khi co tran o T/C2
    //--------------------------------------------------------------------------------

	//dung timer1 lam PWM generator, Fast PWM mode 14: ICR1 chua time period
	TCCR1A=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);
	TCCR1B=(1<<WGM13) |(1<<WGM12) |(1<<CS10) ;
	//--------------------------------------------------------------------------------    

    //**********LCD*************************
    init_LCD();
	clr_LCD();
	print_LCD("Hocavr.com");
    move_LCD(2,1);
    print_LCD("DC Servo Motor");
    _delay_ms(1000);
    clr_LCD();
    print_LCD("Actual :   /25ms");
    move_LCD(2,1);
    print_LCD("Desired:   /25ms");	
    //**********LCD*************************    
    
    //---khoi dong gia tri PWM----------
	OCR1A=1;
	ICR1=PWM_Period;
    sbi(MOTOR_PORT, MOTOR_EN); //khoi dong motor
	sei();
    //----------------------------------
    
	while (1){
		if (sample_count>=10){ //hien thi moi 250ms
            Ctrl_Speed=(PINB>>4) & 0x0F; //doc switches
            
            //------in van toc hien tai (tinh tren 25ms)
			sprintf(dis,"%i", rSpeed);
			move_LCD(1,9);
            print_LCD("  ");
            move_LCD(1,9);
			print_LCD(dis);
            
            //------in van toc mong muon (tinh tren 25ms)
            sprintf(dis,"%i", Ctrl_Speed);
            move_LCD(2,9);
            print_LCD("  ");
            move_LCD(2,9);
			print_LCD(dis);
            
			sample_count=0;
		}
	}
}

ISR (TIMER2_OVF_vect ){		 //update sampling time---------------
	TCNT2=60; //gan gia tri khoi tao cho T/C2	
	sample_count++;
	Motor_Speed_PID(Ctrl_Speed);
}

ISR(INT2_vect){
	if (bit_is_set(PINB,0)) Pulse++;
	else Pulse--;
}


