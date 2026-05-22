# Simon-Says
## Simon Says game for TI MSP430

TI MSP430 used to implement a “Simon Says” game with 4 buttons and 4 LEDs. The LEDs blink in a random sequence that the player repeats using the buttons. Every time the player repeats the pattern correctly, a new random LED is added to the sequence for the next level. Interrupts are used to detect button presses rather than polling. Global interrupts are enabled and edge sensitivity for interrupts is set to any edge. A linear feedback shift register is used with a seed to generate a pseudorandom number from 0-3 for the LED sequence.

Pins P2.1, P2.3, P2.4, P2.5 are configured as inputs for the four game buttons with both hardware and software pull-up resistors, and pins P3.1, P3.3, P3.6, P3.0 are configured as outputs for blue, red, green, and yellow LEDs respectively. Interrupts for buttons and global interrupts are enabled in status register. 

A linear feedback shit register (LSFR) function generate_array() is used with a seed value obtained from a timer to generate pseudorandom values from 0-3 for the game sequence. The current level is shown on the LCD using the displayNum() function from lcd.c and the LED_sequence() function iterates through the current level of the stored sequence array and displays the corresponding LED for each sequence value for approximately 0.5 seconds. The software waits for user input, comparing each button press with the expected sequence. If a mismatch occurs, all LEDs flash for 1.5 seconds to indicate a game over. Successful completion of a level increments level and sequence is replayed with an additional step. If MAX_LEVEL is achieved, all LEDs flash for 0.5 seconds 3 times to indicate that a winner is you.

[Hardware schematic](https://www.circuitlab.com/circuit/vhxwdhpf9446/msp430-simon-says/):
<img width="975" height="384" alt="image" src="https://github.com/user-attachments/assets/cca0ad29-9d28-4a1b-8150-872f1df1b70e" />

Demo:
[![Demo:](https://img.youtube.com/vi/f2eR-FYknnQ/default.jpg)](https://youtu.be/f2eR-FYknnQ)
