#include <io.h>
#include <interrupt.h>
#include <stdlib.h>

#define BIT0                (0x01)
#define BIT1                (0x02)
#define BIT2                (0x04)
#define BIT3                (0x08)
#define BIT4                (0x10)
#define BIT5                (0x20)
#define BIT6                (0x40)
#define BIT7                (0x80)

#define DR          (PORTD = PORTD | BIT5)        // define RS high
#define CWR         (PORTD = PORTD &(~BIT5))      // define RS low
#define READ        (PORTD = PORTD | BIT6)        // define Read signal R/W = 1 for reading
#define WRITE       (PORTD = PORTD &(~BIT6))      // define Write signal R/W = 0 for writing
#define ENABLE_HIGH (PORTD = PORTD| BIT7)        // define Enable high signal
#define ENABLE_LOW  (PORTD = PORTD &(~BIT7))      // define Enable Low signal

void delay(unsigned int k) {
    unsigned int i;
    unsigned int j;
    for(j=0;j<=k;j++) {
        for(i=0;i<100;i++);
    }
}

void dataWrite(void) {
    ENABLE_HIGH;
    delay(2);
    ENABLE_LOW;
}

 
void dataRead(void) {
    ENABLE_LOW;
    delay(2);
    ENABLE_HIGH;
}

 
void checkBusy(void) {
    DDRA &= ~(BIT7); // make PORTA.7 as input
    while((PORTA & BIT7) == 1) {
        dataRead();
    }
    DDRA |= BIT7;  // make P1.7 as output
}

 
void sendCommand(unsigned char cmd) {
        checkBusy();
        WRITE;
        CWR;
        PORTA = (PORTA & 0x00)|(cmd);
        dataWrite();                               // give enable trigger
 
}

 
void sendData(unsigned char data) {
        checkBusy();
        WRITE;
        DR;
        PORTA = (PORTA & 0x00)|(data);
        dataWrite();                               // give enable trigger
}

 
void sendString(char *s) {
    while(*s) {
        sendData(*s);
        s++;
    }
}


void setPosition(unsigned char x, unsigned char y) {
  switch(y) {
    case 0:
      sendCommand((unsigned char)(x|0x80));
      break;
    case 1:
      sendCommand((unsigned char)((0xC0+x)|0x80));
      break;
    case 2:
      sendCommand((unsigned char)((0x90+x)|0x80));
      break;
    case 3:
      sendCommand((unsigned char)((0xD0+x)|0x80));
      break;
  }
}
 
void lcdInit(void) {
		DDRA = 0xFF;	//lcd	
		PORTA = 0x00;	//lcd

		DDRD = 0xFF;	// rs,rw,en
		PORTD = 0x00;	// rs,rw,en

        sendCommand(0x38); // 8 bit mode
        sendCommand(0x0E); // clear the screen
        sendCommand(0x01); // display on cursor on
        sendCommand(0x06); // increment cursor
        sendCommand(0x80); // cursor position
}

#define	poKeysMatrix PORTC
#define	piKeysMatrix PINC
#define	pdKeysMatrix DDRC

volatile struct {
	unsigned SysInt	: 1;
	unsigned T1ms 	: 1;
	unsigned T10ms	: 1;
	unsigned T100ms	: 1;
	unsigned T1s	: 1;
} TimerFlags_Periodic = {0, 0, 0, 0, 0};

#define	fTP_SysInt	TimerFlags_Periodic.SysInt
#define	fTP_1ms		TimerFlags_Periodic.T1ms
#define	fTP_10ms	TimerFlags_Periodic.T10ms
#define	fTP_100ms	TimerFlags_Periodic.T100ms
#define	fTP_1s		TimerFlags_Periodic.T1s

volatile struct{
    unsigned int Previous;
	unsigned int Current;
	unsigned int Pressed;
	unsigned int Released;
}KeysMatrixState = {0, 0, 0};

void initMain(void) {

	// Инициализация таймера
	TIFR = 0;
	OCR0 = 200-1;
	TCCR0 = 0x0A;// Prescaler = 8, CTC, OC1 disconnect
	
	// Инициализация прерываний
	TIMSK = (1 << OCIE0);
	sei();
}

char num[10];
unsigned long total = 0;
char active = 1;

void number(int number) {

	if (total <= 100000000) {
		total = (total * 10)+number;
			setPosition(0,0);
            sprintf(num, "%lu", total); // 10 system
			sendString(num);

			setPosition(0,1);
            sprintf(num, "%lX", total); // 16 system
			sendString(num);

			setPosition(0,2);
            sprintf(num, "%lo", total); // 8 system
			sendString(num);

			setPosition(0,3);
			sendString("Press key B");
	}	else {
		active = 0;
		for (int i = 0; i<=2; i++) {
			total = 0;
			setPosition(0,i);
			sendString("Number>999999999");
		}					   
		setPosition(0,3);
		sendString("Press key A");
	}
}

int main(void) {
volatile unsigned int Temp_KeyPressedPrev = 0;
unsigned char i;

    initMain();
    lcdInit();
	    
    for(;;) {	
        if (KeysMatrixState.Pressed) {//IF
            Temp_KeyPressedPrev = KeysMatrixState.Pressed;
            for (i = 0; i < 16; i++) {
				if (Temp_KeyPressedPrev & (1 << i))
				break;
			}
			
			KeysMatrixState.Pressed = 0;
			if (active) {
				if (i<=10){
					number(i);
				}
			} //active
				if (i == 10) { // переполнение
					lcdInit();
					active = 1;
				}
				if (i == 11) { // enter new number
					lcdInit();
					total = 0;
				}
		}// IF							        
			
	}// for
}

ISR(TIMER0_COMP_vect) {

	static unsigned char Timer_Periodic1ms = 10;
	static unsigned char Timer_Periodic10ms = 100;
	static unsigned int Timer_Periodic100ms = 1000;
	static unsigned int Timer_Periodic1s = 10000;

	register unsigned char KeysMatrixTemp;


	//TIFR |= (1 << OCF0);
	fTP_SysInt = 1;

	if (--Timer_Periodic1ms == 0)
	{
		Timer_Periodic1ms = 10;
		fTP_1ms = 1;
	}
	
	if (--Timer_Periodic10ms == 0)
	{
		Timer_Periodic10ms = 100;
		fTP_10ms = 1;
	}

	if (--Timer_Periodic100ms == 0)
	{
		Timer_Periodic100ms = 1000;
		fTP_100ms = 1;

		KeysMatrixState.Previous = KeysMatrixState.Current;
		// 1 tetrade
		pdKeysMatrix = 0x10;
		asm("nop");
		asm("nop");
		KeysMatrixTemp = ~(piKeysMatrix & 0x0F);
		pdKeysMatrix = 0x20;
		KeysMatrixState.Current = (KeysMatrixState.Current &0xFFF0) | KeysMatrixTemp;
		// 2 tetrade
		KeysMatrixTemp = ~(piKeysMatrix & 0x0F);
		pdKeysMatrix = 0x40;
		KeysMatrixState.Current = (KeysMatrixState.Current &0xFF0F) | (KeysMatrixTemp << 4);
		// 3 tetrade
		KeysMatrixTemp = ~(piKeysMatrix & 0x0F);
		pdKeysMatrix = 0x80;
		KeysMatrixState.Current = (KeysMatrixState.Current &0xF0FF) | (KeysMatrixTemp << 8);
		// 4 tetrade
		KeysMatrixTemp = ~(piKeysMatrix & 0x0F);
		KeysMatrixState.Current = (KeysMatrixState.Current &0x0FFF) | (KeysMatrixTemp << 12);
		pdKeysMatrix = 0x00;
		// common
		KeysMatrixState.Pressed = ~KeysMatrixState.Previous & KeysMatrixState.Current;
		KeysMatrixState.Released = KeysMatrixState.Previous & ~KeysMatrixState.Current;
	}
		
	if (--Timer_Periodic1s == 0) {
			
		Timer_Periodic1s = 10000;
		fTP_1s = 1;		
	}
} // ISR
