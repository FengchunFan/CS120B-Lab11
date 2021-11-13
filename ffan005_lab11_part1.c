/*      Author: ffan005
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo: https:
 */

#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#include <timer.h>
#include <keypad.h> //directly taken from provided header so it is already finished
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {Start,Init};
unsigned char input = 0x00;
unsigned char output = 0x00;

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
                                break;
                        }else if (input == '1'){
                                output = 0x01;
                                break;
                        }else if (input == '2'){
                                output = 0x02;
                                break;
                        }else if (input == '3'){
                                output = 0x03;
                                break;
                        }else if (input == '4'){
                                output = 0x04;
                                break;
                        }else if (input == '5'){
                                output = 0x05;
                                break;
                        }else if (input == '6'){
                                output = 0x06;
                                break;
                        }else if (input == '7'){
                                output = 0x07;
                                break;
                        }else if (input == '8'){
                                output = 0x08;
                                break;
                        }else if (input == '9'){
                                output = 0x09;
                                break;
                        }else if (input == 'A'){
                                output = 0x0A;
                                break;
                        }else if (input == 'B'){
                                output = 0x0B;
                                break;
                        }else if (input == 'C'){
                                output = 0x0C;
                                break;
                        }else if (input == 'D'){
                                output = 0x0D;
                                break;
                        }else if (input == '*'){
                                output = 0x0E;
                                break;
                        }else if (input == '#'){
                                output = 0x0F;
                                break;
                        }else if (input == '0'){
                                output = 0x00;
                                break;
                        }else {
                                output = 0x1B;
                                break;
                        }
                        PORTB = output;
                        break;
                default:
                        break;
        }

        return state;
}


int main(void){
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;

    static task task1;
    task *tasks[] = { &task1 };
    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
    const char start = -1;

    task1.state = start;
    task1.period = 50;
    task1.elapsedTime = task1.period;
    task1.TickFct = &;

    TimerSet(100);
    TimerOn();

    unsigned short i;
    while (1) {
        for (i = 0; i < numTasks; ++i) {
                if (tasks[i]->elapsedTime == tasks[i]->period) {
                        tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                        tasks[i]->elapsedTime = 0;
                }
                tasks[i]->elapsedTime += 50;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}
