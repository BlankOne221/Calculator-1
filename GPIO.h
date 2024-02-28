#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

// Register addresses asssoicated with pins of PMODB port.
#define DATA_2 0xE000A048
#define DATA_2_RO 0xE000A068
#define DIR_2 0xE000A284
#define OE_2 0xE000A288

#define PMODB_TOP_MASK 0x0780
#define PMODB_BOTTOM_MASK 0x7800

// All the defines for the seven segment display
#define SVN_SEG_CNTRL_BASEADDR 0x43C10000  // Base address of Seven Segment Control
#define SVN_SEG_DATA_BASEADDR 0x43C10004   // Base address of Seven Segment Data

#define BUTTONS_BASEADDR 0x41200000		// Base address of Buttons IP Rev. D 2020.2 XSA
#define SWITCHES_BASEADDR 0x41220000   // Base address of Switches

// Setup a PMODB row as either input or output
// row = 0 means PMODB row, row = 1 means PMODB bottom row
// inout = 0 means that row is an input, inout = 1 means that row is an output

void setupPMODB(unsigned int inout, unsigned int row);

// Write the value to a PMODB output row 
void writePMODB(unsigned int value, unsigned int row);

// Read the value from PMODB input row
unsigned int readPMODB(unsigned int row);

// Check Keypad to see which key is turned on
unsigned int getKeyPad_data();

// Display the data from KeyPad to the Seven segment display
unsigned int displayKeyPad();

// Seven segment display function
void display(unsigned int input);

// get buttons state function
unsigned int get_button_states(void);

// get switches state funciton
unsigned int get_switches_states(void);

// switch statements operations for the calculator
unsigned int Operations(unsigned int Op1, unsigned int Op2, unsigned int *Stored, unsigned int temp);

#endif