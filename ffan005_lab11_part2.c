/*      Author: ffan005
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo: 
 */

#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#include <timer.h>
#include <keypad.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


enum States {Start, Init};
unsigned char MSG[40]={'C','S','1','2','0','B',' ','i','s',' ','L','e','g','e','n','d','.','.','.','w','a','i','t',' ','f','o','r',' ','i','t',' ','D','A','R','Y','!',' ',' ',' ',' '};

unsigned char i = 0x00;
unsigned char j;
unsigned char display[16];

int Tick(int state) {
        switch(state) {
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
        switch(state) {
                case Start:
                        break;
                case Init:
                                for (int j = 0; j < 16; ++j) {
                                        display[j] = MSG[j + i];
                                }
                                i++;
                                if(i == 39){
                                i = 0;
                                }
                                LCD_DisplayString(1, display);
                                break;
                default:
                                break;
        }

        return state;
}

int main(void) {
    DDRA = 0xF0; PORTA = 0x0F;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    LCD_init();

    static task task1;
    task *tasks[] = { &task1 };
    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
    const char start = 0;

    task1.state = start;
    task1.period = 8;
    task1.elapsedTime = task1.period;
    task1.TickFct = &Tick;

    TimerSet(8);
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

