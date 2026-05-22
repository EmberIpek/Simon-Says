// Ember Ipek
//
// MSP430 Simon Says game using 4 buttons and 4 LEDs. The LEDs blink in a random sequence
// that the player repeats using the buttons. Every time the player repeats the pattern
// correctly, a new random LED is added to the sequence for the next level. Interrupts are
// used to detect button presses rather than polling. Global interrupts are enabled and
// edge sensitivity for interrupts is set to any edge. A linear feedback shift register is
// used with a seed to generate a pseudorandom number from 0-3 for the LED sequence.

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <lcd.h>

#define MAX_LEVEL 10

int sequence[MAX_LEVEL];
int LFSR, counter = 0, seed;
int level = 1;

uint16_t update_LFSR(uint16_t LFSR)
{

  uint16_t new_val;

  new_val  = ((LFSR & BIT0)) ^  //create new bit to be rotated in
             ((LFSR & BIT1) >> 1) ^
             ((LFSR & BIT3) >> 3) ^
             ((LFSR & BIT5) >> 5);

  LFSR = LFSR >> 1;             //shift to perform rotation
  LFSR &= ~(BITF);              //have to clear bit because shift is arithmetic
  LFSR |= (new_val << 15);      //combine with new bit

  return LFSR;
}

void msp_init()
{
    WDTCTL = WDTPW | WDTHOLD;     // Stop watchdog timer

    P2DIR &= ~(BIT1 | BIT3 | BIT4 | BIT5);

    P2REN |=  (BIT1 | BIT3 | BIT4 | BIT5);
    P2OUT |=  (BIT1 | BIT3 | BIT4 | BIT5);

	P3DIR |=  (BIT0 | BIT1 | BIT3 | BIT6);

    PM5CTL0 &= ~LOCKLPM5;         // Unlock ports from power manager

    P2IES |=  (BIT1 | BIT3 | BIT4 | BIT5);
    P2IFG &= ~(BIT1 | BIT3 | BIT4 | BIT5);
    P2IE  |=  (BIT1 | BIT3 | BIT4 | BIT5);

    __enable_interrupt();         // Set global interrupt enable bit in SR register
}

void delay(void) {
    volatile unsigned loops = 50000; // Start the delay counter at 25,000
    while (--loops > 0);             // Count down until the delay counter reaches 0
}

void debounce() {
    volatile int i;
    for(i=0; i<500; i++);
}

#pragma vector = PORT2_VECTOR
__interrupt void p2_ISR()
{
    debounce();

	switch (P2IV)
	{
        //blue
        case P2IV_P2IFG1:
//            P3OUT ^= BIT1;
            if (P2IN & BIT1) {
                P3OUT &= ~BIT1;
                P2IES |= BIT1;
            } else {
                P3OUT |= BIT1;
                P2IES &= ~BIT1;
            }

            break;
        //red
        case P2IV_P2IFG3:
//            P3OUT ^= BIT3;
            if (P2IN & BIT3) {
                P3OUT &= ~BIT3;
                P2IES |= BIT3;
            } else {
                P3OUT |= BIT3;
                P2IES &= ~BIT3;
            }

            break;
        //green
        case P2IV_P2IFG4:
//            P3OUT ^= BIT6;
            if (P2IN & BIT4) {
                P3OUT &= ~BIT6;
                P2IES |= BIT4;
            } else {
                P3OUT |= BIT6;
                P2IES &= ~BIT4;
            }

            break;
        //yellow
        case P2IV_P2IFG5:
//            P3OUT ^= BIT0;
            if (P2IN & BIT5) {
                P3OUT &= ~BIT0;
                P2IES |= BIT5;
            } else {
                P3OUT |= BIT0;
                P2IES &= ~BIT5;
            }

            break;

        default: break;
	}
}

//Write a function that uses gives this seed value for the LFSR and runs the given LFSR routine
//(as shown in the code walk-through) to generate 32 random 2 bit values and stores them, or
//unique values based upon them in an array. This array now holds a sequence of 32 random
//values.
void generate_array(int seed){
    //use seed to generate LFSR
    LFSR = seed;

    int i;
    for(i = 0; i < MAX_LEVEL; i++){
        LFSR = update_LFSR(LFSR);
        LFSR = update_LFSR(LFSR);
        sequence[i] = LFSR & (BIT0 | BIT1);
    }

    return;
}

//Write a function that shows this random sequence on the LEDs. The LED corresponding to
//the sequence value should light up for approximately half a second. Then all LEDs should
//be off for a fraction of that after which the LED corresponding to the next sequence value
//should light up and so on. After the complete sequence was shown on the LEDs, it should
//start all over again.
void LED_sequence(){
    int i;
    for(i = 0; i < level; i++){
        switch(sequence[i]){
            case 0:
                //blink blue
                P3OUT |= BIT1;
                delay();
                P3OUT &= ~BIT1;
                delay();

                break;
            case 1:
                //blink red
                P3OUT |= BIT3;
                delay();
                P3OUT &= ~BIT3;
                delay();

                break;
            case 2:
                //blink green
                P3OUT |= BIT6;
                delay();
                P3OUT &= ~BIT6;
                delay();

                break;
            case 3:
                //blink yellow
                P3OUT |= BIT0;
                delay();
                P3OUT &= ~BIT0;
                delay();

                break;
            default:
                break;
        }
    }

    return;
}

//check which button is pressed
int button_pressed(){
    if(!(P2IN & BIT1))
        return 0;
    if(!(P2IN & BIT3))
        return 1;
    if(!(P2IN & BIT4))
        return 2;
    if(!(P2IN & BIT5))
        return 3;

    return -1;
}

void main(void)
{
	msp_init();
	lcd_init();
	lcd_clear();

    while(1){

//    	__bis_SR_register(LPM3_bits);  // Enter low power mode

//    	In the beginning of your program, run a counter with a short delay loop that increments until
//    	any button is pressed. The counter value at that time shall be the seed value for the LFSR
    	while(1){
            counter++;
            if((P2IN & (BIT1|BIT3|BIT4|BIT5)) != (BIT1|BIT3|BIT4|BIT5)) {
                seed = counter;

                break;
            }
        }
    	lcd_clear();

    	//generate random sequence
    	generate_array(seed);

    	//start at level 1
    	level = 1;

    	while(level <= MAX_LEVEL){
    	    //wait for 1 second
    	    delay();
    	    delay();

    	    displayNum(level);
    	    //display the sequence up to level
    	    LED_sequence();

    	    //level++ when a button is pressed
//    	        Get user input after displaying a sequence and compare each button press to the expected
//    	        button

            /////////////////////////////////////////////////////////////////////////////////

            //if button is pressed, check which button
            int i;
            int game_over = 0;
            for(i = 0; i < level; i++){
                //wait for button press
                while((P2IN & (BIT1|BIT3|BIT4|BIT5)) == (BIT1|BIT3|BIT4|BIT5));
                if(sequence[i] != button_pressed()){
                    //wait for button release
                    while((P2IN & (BIT1|BIT3|BIT4|BIT5)) != (BIT1|BIT3|BIT4|BIT5));
                    //level = 0 because increment outside of loop
//                    level = 0;
                    game_over = 1;
                    break;
                }
                //wait for button release
                while((P2IN & (BIT1|BIT3|BIT4|BIT5)) != (BIT1|BIT3|BIT4|BIT5));
            }

            //better to exit loop and start with new seed
            if(game_over == 1){
                P3OUT |= (BIT1|BIT3|BIT6|BIT0);
                delay();
                delay();
                delay();
                P3OUT &= ~(BIT1|BIT3|BIT6|BIT0);
                break;
            }

            //a winner is you
            if(level == MAX_LEVEL){
                P3OUT |= (BIT1|BIT3|BIT6|BIT0);
                delay();
                P3OUT &= ~(BIT1|BIT3|BIT6|BIT0);
                delay();

                P3OUT |= (BIT1|BIT3|BIT6|BIT0);
                delay();
                P3OUT &= ~(BIT1|BIT3|BIT6|BIT0);
                delay();

                P3OUT |= (BIT1|BIT3|BIT6|BIT0);
                delay();
                P3OUT &= ~(BIT1|BIT3|BIT6|BIT0);
                delay();
            }

            level++;

            /////////////////////////////////////////////////////////////////////////////////

            //if any button pressed
//            if((P2IN & (BIT1|BIT3|BIT4|BIT5)) != (BIT1|BIT3|BIT4|BIT5)) {
//                level++;
//
//                //wait for button release
//                //do we really need 3 nested while loops???
//                while(1){
//                    if((P2IN & (BIT1|BIT3|BIT4|BIT5)) == (BIT1|BIT3|BIT4|BIT5)){
//                        break;
//                    }
//                }
//                break;
//            }

//            level = 32;
//            LED_sequence();
            //////////////////////////////////////////////////////////////////////////////////

    	}

//    	__bis_SR_register(LPM3_bits);  // Enter low power mode

    }

}
