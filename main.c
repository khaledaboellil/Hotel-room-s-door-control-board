#include "stdint.h"
#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>
#include "E:/keil/EE319Kware/inc/tm4c123gh6pm.h"
void SystemInit(){}
# define passwordsize 4
# define maxroom 20 // Max 20 rooms
//==================Declare Function == =====//
void keypad_init(void);
unsigned char keypad_kbhit(void);
void keypad_init(void);
unsigned char keypad_getkey(void);
void Getpassword(char * passkey); /*Get password from
keypad */
void portF_led_init(void) ;
void uart_init(void);
char UART0_ReadChar(void) ; //recive char from pc
int UART0_Recivestringtointconvert(void) ;/*recive string
from pc and covert it to number using atoi()*/
int setup () ;
void systick_init(void);
void systick_delay_us (uint32_t delay);
//===============Global Variables==================//
uint32_t dealy ; // dummy variable
uint16_t i =0 ;
uint16_t b =0 ;
uint16_t norooms =0 ;
uint16_t flag =1 ;/*flag when compare two passwords from
keypad and pc*/
int roomnumber ;
int status [maxroom] ; // array store status of rooms
char setpass[maxroom][passwordsize]; /*2D array store
password four numbers for every room */
char passkey[passwordsize]; /*get password from keypad and store it in this array*/
//==============Main====================//
int main()
{
keypad_init();
uart_init();
portF_led_init();
systick_init();
norooms =setup(); /* initialize rooms and return how
many rooms and assume it start from room zero*/
while(1)
{
flag=1 ;
roomnumber=UART0_Recivestringtointconvert();
//Read room ID
status[roomnumber]=UART0_Recivestringtointconvert();
//Read status for this room
if(status[roomnumber]==1) /*if status equal 1 set the
password to the room */
{for( i=0 ;i<4;i++) /* set password for room . password size is four numbers*/
{
setpass[roomnumber][i]=UART0_ReadChar() ;
}
}
//=====test keypad and solenoid for room number 3
if(roomnumber==3)/*iconnected keypad and solenoid
only in room 3 bc we have only 1 keypad and 1solenoid*/
{
if (status[roomnumber]==1) // if status check in
{
Getpassword( passkey) ; //get password from keypad
for ( i=0; i<passwordsize; i++){
if(setpass[roomnumber][i] != passkey[i]) /*check if the
password is correct*/
flag=0 ; // flag =0 if password incorrect
}
if(flag) //if password valid
GPIO_PORTF_DATA_R=0x02 ; //open solenoid
else //if password not valid
GPIO_PORTF_DATA_R=0 ; // solenoid is closed
}
else if (status[roomnumber]==2) //if status cleaning
{
GPIO_PORTF_DATA_R=0x02 ; //open solenoid
}
else //if status check out
{
GPIO_PORTF_DATA_R=0 ; //close solenoid
}
}
}
}
//============= initialize led as a solenoid ========//
void portF_led_init(void)
{
SYSCTL_RCGCGPIO_R |= 0x20;
dealy =1 ;
GPIO_PORTF_LOCK_R=0x4C4F434B;
GPIO_PORTF_CR_R=0x1F ;
GPIO_PORTF_DIR_R=0x02;
GPIO_PORTF_DEN_R=0x02 ;
GPIO_PORTF_AFSEL_R=0;
GPIO_PORTF_PCTL_R=0 ;
GPIO_PORTF_AMSEL_R=0;
}
//==============initilaize uart A0 as a reciver=====//
void uart_init(void)
{
SYSCTL_RCGCUART_R |= 0x0001; // enable UART0
SYSCTL_RCGCGPIO_R |= 0x0001; // enable port A
UART0_CTL_R &= ~0x0001; // deactivate UART
UART0_IBRD_R = 520; //boudrate9600
UART0_FBRD_R = 53;
UART0_LCRH_R = 0x0070; //8-bitword length, enable FIFO
UART0_CTL_R = 0x0201; // enable RXE and UART
GPIO_PORTA_AFSEL_R |= 0x01; // enable alt function PA0
GPIO_PORTA_PCTL_R=(GPIO_PORTA_PCTL_R&0xFFFFFFF0)
+0x00000001;
/* configure UART for PA0 as a reciver */
GPIO_PORTA_DEN_R |= 0x01; // enable digital I/O on PA0
GPIO_PORTA_AMSEL_R &= ~0x01; // disable analog
function on PA0
}
//=============Receive char from Pc ========//
char UART0_ReadChar(void){
while((UART0_FR_R & 0x0010) != 0); /*check if the buffer
is empty*/
return (char)(UART0_DR_R & 0xFF); /*return the first 8
bits (Data)*/
}
//====Receive string and covert it into number==//
int UART0_Recivestringtointconvert(void)
{
char array[2] ;
uint16_t n =0 ;
while(UART0_ReadChar() != '\0')
{
array[n]=UART0_ReadChar();
n++ ;
}
return atoi(array) ; /*return number by using atoi to
covert string to number*/
}
//=================timer initiliaze========//
void systick_init(void){
NVIC_ST_CTRL_R = 0;
NVIC_ST_RELOAD_R = 0x00FFFFFF;
NVIC_ST_CURRENT_R =0;
NVIC_ST_CTRL_R=0x05;
}
void systick_delay_us (uint32_t delay){
uint32_t i;
for (i=0;i<delay;i++)
{
NVIC_ST_RELOAD_R = 80 -1;
NVIC_ST_CURRENT_R = 0;
while ( (NVIC_ST_CTRL_R & 0x00010000) == 0){};
}
}
//=======Keypad initiliaze=================//
void keypad_init(void)
{
SYSCTL_RCGCGPIO_R |= 0x04;// active clock to C
dealy=1 ;
SYSCTL_RCGCGPIO_R |= 0x10;// active clock to E
dealy=1 ;
GPIO_PORTE_DIR_R|= 0x0F;// pins 3-0 as output
GPIO_PORTE_DEN_R|= 0x0F;// pins 3-0 as digital pins
GPIO_PORTE_ODR_R|= 0x0F; // pins 3-0 as open drain
GPIO_PORTC_DIR_R&=~0xF0;// pin 7-4 as input */
GPIO_PORTC_DEN_R|= 0xF0; // pin 7-4 as digital pins */
GPIO_PORTC_PUR_R |= 0xF0; /* enable pull-ups resistors
for pin 7-4 */
}
//=======keypad get char===================//
unsigned char keypad_getkey(void)
{
const unsigned char keymap[4][4] = {
{ '1', '2', '3', 'A'},
{ '4', '5', '6', 'B'},
{ '7', '8', '9', 'C'},
{ '*', '0', '#', 'D'},
};
int row, col;
GPIO_PORTE_DATA_R = 0; /* enable all rows */
col = GPIO_PORTC_DATA_R & 0xF0; /* read all columns */
if (col == 0xF0) return 0; /* no key pressed */
while (1)
{
row = 0;
GPIO_PORTE_DATA_R = 0x0E; /* enable row 0 */
systick_delay_us (2);
col = GPIO_PORTC_DATA_R & 0xF0;
if (col != 0xF0) break;
row = 1;
GPIO_PORTE_DATA_R = 0x0D; /* enable row 1 */
systick_delay_us (2);
col = GPIO_PORTC_DATA_R & 0xF0;
if (col != 0xF0) break;
row = 2;
GPIO_PORTE_DATA_R = 0x0B; // active row 2
systick_delay_us (2);
col = GPIO_PORTC_DATA_R & 0xF0;
if (col != 0xF0) break;
row = 3;
GPIO_PORTE_DATA_R = 0x07; // active row 3
systick_delay_us (2);
col = GPIO_PORTC_DATA_R & 0xF0;
if (col != 0xF0) break;
return 0; /* if no key is pressed */
}
if (col == 0xE0) return keymap[row][0]; // key in column 0
if (col == 0xD0) return keymap[row][1]; // key in column 1
if (col == 0xB0) return keymap[row][2]; // key in column 2
if (col == 0x70) return keymap[row][3]; //key in column 3
return 0;
}
//===============return if key is pressed====//
unsigned char keypad_kbhit(void)
{
int col;
GPIO_PORTE_DATA_R= 0; // active all rows
col = GPIO_PORTC_DATA_R & 0xF0; // read all columns
if (col == 0xF0)
return 0; // no key pressed
else
return 1; // a key is pressed
}
