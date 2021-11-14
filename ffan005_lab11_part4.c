/*      Author: ffan005
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo: https://www.youtube.com/watch?v=2txJqcir8SY
 */

#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#include <timer.h>
#include <keypad.h> //modified to make pad connect to PINA
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {Start,Init};
unsigned char input = 0x00;
unsigned char output = 0x00;

//code from part 1 to verify correctness
int Tick(int state){
        input = GetKeypadKey();
        switch(state){
                case Start:
                        state = Init;
                        break;
                case Init:
                        state = Init;
                        break;
                default:
                        state = Start;
                        break;
        }
  
        switch(state){
                case Start:
                        break;
                case Init:
                        if(input == '\0'){
                                output = 0x1F;
                        }else if (input == '1'){
                                output = 0x01;
                        }else if (input == '2'){
                                output = 0x02;
                        }else if (input == '3'){
                                output = 0x03;
                        }else if (input == '4'){
                                output = 0x04;
                        }else if (input == '5'){
                                output = 0x05;
                        }else if (input == '6'){
                                output = 0x06;
                        }else if (input == '7'){
                                output = 0x07;
                        }else if (input == '8'){
                                output = 0x08;
                        }else if (input == '9'){                   
                                output = 0x09;
                        }else if (input == 'A'){
                                output = 0x0A;
                        }else if (input == 'B'){
                                output = 0x0B;
                        }else if (input == 'C'){
                                output = 0x0C;
                        }else if (input == 'D'){
                                output = 0x0D;
                        }else if (input == '*'){
                                output = 0x0E;
                        }else if (input == '#'){
                                output = 0x0F;
                        }else if (input == '0'){
                                output = 0x00;
                        }else {
                                output = 0x1B;
                        }
                        //PORTB = output;
                        break;
                default:
                        break;
        }

        return state;
}

enum States2 {Start2, Init2, Pause};
unsigned char i = 0x00;

int Tick2(int state) {
	switch(state) {
		case Start2:
      			state = Init2;
			break;
		case Init2:
     			if(output == 0x1F) {
        		state = Init2;
     			 }	else {
        		state = Pause;
      			}
			break;
		case Pause:	
     			 state = Init2;
		default: 	
      			state = Start2;
			break;
	}
  
	switch(state) {
		case Start2:	
     			 break;
		case Init2:	
			break;
		case Pause:	
      			if(i ==16){
               i = 1;
            }else{
            i++;
            }
            LCD_Cursor(i);
            LCD_WriteData(input);
      break;      
		default:	
			break;
	}
	return state;
}

int main(void) {
    DDRA = 0x0F; PORTA = 0xF0;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    LCD_init();
    LCD_DisplayString(1, "Congratulations!");
  
    static task task1, task2;
    task *tasks[] = { &task1, &task2 };
    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
    const char start = -1;

    task1.state = start;
    task1.period = 4;
    task1.elapsedTime = task1.period;
    task1.TickFct = &Tick;

    task2.state = start;
    task2.period = 6;
    task2.elapsedTime = task2.period;
    task2.TickFct = &Tick2;

    TimerSet(1);
    TimerOn();

    unsigned short i;
    while (1) {
	for (i = 0; i < numTasks; ++i) {
		if (tasks[i]->elapsedTime == tasks[i]->period) {
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += 1;
	}
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
