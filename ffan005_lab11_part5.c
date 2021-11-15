/*      Author: ffan005
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *      Demo: https://www.youtube.com/watch?v=Adsa5IOeGbo
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

//creating two states, one taking care of button parts and one with LCD 
//using both lines meaning we need 1-32 positions, and moving the cursor from 1 to 17

unsigned char i = 1; //position indicator for cursor, should only change between 1 and 17
enum States {Button_Start,Button_Init,Up,Down,Release};//no pause, button shall only control cursor, using PINA as input

int Button_Tick(int state){
switch(state){
        case Button_Start:
                state = Button_Init;
                break;
        case Button_Init:
                if((~PINA & 0x07) == 0x02){
                        state = Up;
                }else if((~PINA & 0x07) == 0x04){
                        state = Down;
                }else{
                        state = Button_Init;
                }
                break;
        case Up:
                state = Release;
                break;
        case Down:
                state = Release;
                break;
        case Release:
                if((~PINA & 0x07) == 0x00){
                        state = Button_Init;
                }else{
                        state = Release;
                }
                break;
        default:
                state = Button_Start;
                break;

}

switch(state){
        case Button_Start:
                break;
        case Button_Init:
                break;
        case Up:
                i = 1;
                break;
        case Down:
                i = 17;
                break;
        case Release:
                break;
        default:
                break;
}

return state;
}

enum LCD_States{LCD_Start, LCD_Init, LCD_Pause, LCD_Lose, LCD_Release};
unsigned char game[] = "       #                   #    ";
unsigned char lose[] = "you lost heheheh";
unsigned char j = 0; //scroll the game message

int LCD_Tick(int state){
switch(state){
        case LCD_Start:
                state = LCD_Init;
                break;
        case LCD_Init:
                if((~PINA & 0x07) == 0x01){
                        state = LCD_Pause;
                }else if(game[i + j] == '#'){
                        state = LCD_Lose;
                }else{
                        state = LCD_Init;
                }
                break;
        case LCD_Pause:
                state = LCD_Release;
                break;
        case LCD_Lose:
                if((~PINA & 0x07) == 0x01){
                        state = LCD_Init;
                }else{
                        state = LCD_Lose;
                }
                break;
        case LCD_Release:
                if((~PINA & 0x07) == 0x00){
                        state = LCD_Init;
                }else{
                        state = LCD_Release;
                }
                break;
        default:
                state = LCD_Start;
                break;

}

  switch(state){
        case LCD_Start:
                break;
        case LCD_Init:
                LCD_Cursor(i); //player
                //LCD_DisplayString(i,6);

                for(int a = 1; a <= 32; ++a){ //evil #
                        LCD_Cursor(a);
                        LCD_WriteData(game[(a+j)%32]);
                }

                if(j == 32){
                        j = 0;
                }else{
                        j++;
                }
                break;
        case LCD_Pause:
                break;
        case LCD_Lose:
                LCD_DisplayString(1,lose);
                i = 1;
                j = 0;
                break;
        case LCD_Release:
                break;
        default:
                break;

}

return state;
}

int main(void){
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    LCD_init();

    static task task1, task2;
    task *tasks[] = { &task1, &task2 };
    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
    const char start = -1;

    task1.state = start;
    task1.period = 1;
    task1.elapsedTime = task1.period;
    task1.TickFct = &Button_Tick;

    task2.state = start;
    task2.period = 2;
    task2.elapsedTime = task2.period;
    task2.TickFct = &LCD_Tick;

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

