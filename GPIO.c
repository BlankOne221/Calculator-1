#include "GPIO.h"
#include <unistd.h>    // Include this library to use usleep()

// Setup a PMODB row as either input or output
// row = 0 means PMODB row
// row = 1 means PMODB bottom row
// inout = 0 means that row is an input 
// inout = 1 means that row is an output

void setupPMODB(unsigned int inout, unsigned int row) {

    if(row == 0) {
        if(inout == 0) { // PMODB top row as an input
            // PMODB_TOP_MASK 0x0780
            // DIR_2 register = ....xxxx0000xxxx.... bits 7-10 of this register
            (* (unsigned int*) DIR_2) &= ~PMODB_TOP_MASK;
            (* (unsigned int*) OE_2) &= ~PMODB_TOP_MASK;

        // PMOD_BOTTOM_MASK 0x7800
        }
        else {   // PMODB top row as an output
            // PMODB_TOP_MASK 0x0780
            // DIR_2 register = ....xxxx1111xxxx.... bits 7-10 of this register
            (* (unsigned int*) DIR_2) |= PMODB_TOP_MASK;
            (* (unsigned int*) OE_2) |= PMODB_TOP_MASK;            
        }
    }
    else {
        if(inout == 0) {  // PMODB bottom row as an input
            // PMODB_BOTTOM_MASK 0x7800
            // DIR_2 register = ....xxxx0000xxxx.... bits 11-14 of this register
            (* (unsigned int*) DIR_2) &= ~PMODB_BOTTOM_MASK;
            (* (unsigned int*) OE_2) &= ~PMODB_BOTTOM_MASK;
        }
        else {
            // PMODB_BOTTOM_MASK 0x7800
            // DIR_2 register = ....xxxx1111xxxx.... bits 11-14 of this register
            (* (unsigned int*) DIR_2) |= PMODB_BOTTOM_MASK;
            (* (unsigned int*) OE_2) |= PMODB_BOTTOM_MASK;
        }
    }

    return;

}

// This function activiates each colum and checks the rows inside the columns on the keypad
// for a button press
// returns: temp
unsigned int getKeyPad_data() {

    unsigned short temp = 0;
    int i = 0;
    
    for (i = 0; i < 4; i++) {

        // writing to each column inside the loop
        writePMODB(~(0b0001 << i),0);
        // short sleep function
        usleep(100);

        temp = temp << 4;
        // reading the rows
        temp |= readPMODB(1);

    }

    return temp;

}

// takes the data from getKeyPad_data() and shifts through the temp return variable looking
// for a button press, once found it will count the shifts and refrence the keypad array
// to get the correct keypad number, then display it using the display function at the bottom
// it also returns the correctly pressed button
unsigned int displayKeyPad() {

    // our keypad array and initizlations 
    int keypad[17] = {0, 7, 4, 1, 15, 8, 5, 2, 14, 9, 6, 3, 13, 12, 11, 10, 0};
    int i = 0;
    unsigned short temp1 = 0;
    unsigned short temp2 = 0;


    // while loops for waiting for a keypad press and release
    do {
        temp1 = getKeyPad_data();
        temp1 = (~temp1);
    } while(temp1 == 0);

    do {
        temp2 = getKeyPad_data();
        temp2 = (~temp2);
    } while(temp2 == temp1);

    // for loop for checking temp1 and which digit the keypad press is in, and using
    // i variable as the count 
    for(i=0; i<16; i++) {

       if(temp1 != 0) {
           temp1 = temp1 >> 1;

       } else {
           break;
       }
        
    }
   
    // display the correct press
    display(keypad[i-1]);

    // return the keypad press
    return keypad[i-1];

}

// Basically the calculator function, which takes 4 inputs, our two operands from pressing the keypad twice
// and our stored and temp variable for our stored calculations. Once the operands have been put in, mySwitches
// gets the switches states and goes to the correct switch statment. Then in the end it displays it on the seven seg
// and returns the solution.
unsigned int Operations(unsigned int Op1, unsigned int Op2, unsigned int *Stored, unsigned int temp) {

    // initilzation of variables and mySwitches which has the switches states
    unsigned int mySwitches = get_switches_states();
    int Sol = 0;
    unsigned int i = 0;
    
    // Big switch case code for all the different operations of the calculator
    switch (mySwitches) {
        case 0b0000:
            Sol = Op1 + Op2;
            break;

        case 0b0001: 
            Sol = Op1 - Op2;
            break;
            
        case 0b010:
            Sol = Op2 - Op1;
            break;

        case 0b011:
            Sol = Op1 * Op2;
            break;

        case 0b0100:
            Sol = (Op1 * Op2) + *Stored;
            *Stored = (Op1 * Op2) + *Stored;
            break;

        case 0b0101:
            if(Op1 == Op2) {
                Sol = 1;

            } else {
                Sol = 0;
            }
            break;
        
        case 0b0110:
            Sol = Op1 << Op2;
            break;

        case 0b0111:
            Sol = Op1 >> Op2;
            break;

        case 0b1000:
            Sol = Op1 & Op2;
            break;

        case 0b1001:
            Sol = Op1 | Op2;
            break;

        case 0b1010:
            Sol = Op1 ^ Op2;
            break;

        case 0b1011:
            Sol = Op1 & ~Op2;
            break;

        case 0b1100:
            Sol = Op1;
            break;

        // our leading zero code
        case 0b1101:
           for(i=0; i<32; i++) {
                if(Op1 != 0) {
                 Op1 = Op1 >> 1;

                } else {
                    break;
                }
           }

            Sol = 32 - i; 
            break; 

        case 0b1110:
            *Stored = temp;
            Sol = temp;            
            break; 

        case 0b1111:
            Sol = *Stored;                 
            break;

        default: 

            break;

    } 

    // display and return the solution
    display(Sol);

    return Sol;
}




// Write the value to a PMODB output row
void writePMODB(unsigned int value, unsigned int row) {
    if (row == 0) {
        // We want to only MODIFY the bits located at 7-10 of DATA_register
        // ...xxxxYYYYxxxx...
        // First 0 out bits 7-10, then write 1's where needed
       (* (unsigned int *) DATA_2) = ((*((unsigned int *) DATA_2) & ~PMODB_TOP_MASK) | (((value << 7)) & PMODB_TOP_MASK));
    }
    else {
        (* (unsigned int *) DATA_2) = ((*((unsigned int *) DATA_2) & ~PMODB_BOTTOM_MASK) | (((value << 11)) & PMODB_BOTTOM_MASK));

    }

}

// Read the value from a PMODB input row
unsigned int readPMODB(unsigned int row) {
    if (row == 0) {
        return (*((unsigned int *)DATA_2_RO) & PMODB_TOP_MASK) >> 7;     

    }
    else {
        return (*((unsigned int *)DATA_2_RO) & PMODB_BOTTOM_MASK) >> 11;

    }

}

// A simple display function that turns on our display, then using modolus and divison it takes
// the decimal input and shifts it occordingly to the correct place on the seven segment display 
// and displays it.
void display(unsigned int input) {

    unsigned int temp1 = 0;
    *((unsigned int *) SVN_SEG_CNTRL_BASEADDR) = 0b01;

    for(int i = 0; i<4; i++) {

        // Takes input from programmer and modolus it and then shifts to correct place 
        temp1 |= (input % 10)<<(i*8);
        input /= 10;

    }

    // Removing the decimals
    temp1 |= 0x80808080;

    // Sending temp to SVN_SEG 
    *((unsigned int *)SVN_SEG_DATA_BASEADDR) = temp1;
  
}

/*************************************************************
 * Function: get_button_states ()                            *
 * Date Created: January 17, 2019                            *
 * Date Last Modified: January 30, 2021                      *
 * Description: This function reads the state of the         *
 *              on-board buttons. 1 - indicates button       *
 *              is pressed, 0 - otherwise                    *
 * Input parameters: None                                    *
 * Returns: Values of BTNs                                   *
 * Usages: Must be called before outputting to LEDs			 *
 * Preconditions: None                                       *
 * Postconditions: Masked state of buttons                   *
 *************************************************************/

unsigned int get_button_states(void) {
	unsigned int button_states = 0;
	// Read the entire 32-bit BUTTONS_BASEADDR register located at 0x41200000
	// Mask the bits so that the corresponding button bits are isolated;
	// We do not care about the other 28-bits of BUTTONS_BASEADDR;
	// Little Endian is used: bit31 bit30 bit29 ... bit02 bit01 bit00
	// 1 hex digit = 1 nibble = 4 bits
	button_states = *((unsigned int *)BUTTONS_BASEADDR);
	button_states &= 0b1111;

	return button_states;
}

/*
Function: get_switches_states
Designed by: Artsem Labanau
Description: This function grabs the state of the switches and return is to the function call.
*/

unsigned int get_switches_states(void) {

	unsigned int switches_states = 0;
	switches_states = *((unsigned int *)SWITCHES_BASEADDR);
	switches_states &= 0b111111111111;

	return switches_states;
}