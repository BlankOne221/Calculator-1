#include "GPIO.h"
#include <unistd.h>    // Include this library to use usleep()


int main (void) {

    // Initilizing all the variables
    int myButtons = 0;
    unsigned int short Op1 = 0;
    unsigned int short Op2 = 0;
    unsigned int Store = 0;
    unsigned int temp5 = 0;

    setupPMODB(1,0); // top row is an input
    setupPMODB(0,1); // bottom row is an output


    while(1) {

        // Get the first operand and display it 
        Op1 = displayKeyPad();

        // Get the second operand and display it
        Op2 = displayKeyPad();
        
        // call operations which is our calculator and store the sol into a temp
        temp5 = Operations(Op1,Op2,&Store,temp5);

        // waits for a press and a release of buttons before moving on
        do {
            myButtons = get_button_states();
        } while( myButtons == 0);  

        do {
            myButtons = get_button_states();
        } while( myButtons != 0);

        // resets the display
        display(0000);

    }

    return 0;
}

