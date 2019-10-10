/****************************************************************************
  * Title: Mastermind Game *
  * Authors: Dominic Calina, Matthew Reilly *
  ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//used to output to the LCD screen by using command lcd('string','string')
//can also use Clear() to clear LCD screen.
#include "lcd.h"

//used for the input of the codes by using command inputbutton("delay")
#include "buttoninput.h"

//used to blink the red LED once by using command blinkred()
#include "blinkred.h"

//used to blink the green(yellow) LED once by using command blinkyellow()
#include "blinkyellow.h"

//used to pause the game until the button is pressed by using command waitforpress()
#include "buttonwait.h"

//used to turn on and off red led by using command redon or redoff
#include "red.h"

//defines the max digits in the code
#define MAX 3

//defines the max size of each digit in the code (usually 3)
#define MAXNUM 3

//declaration of global variables
int exact, approximate, attempts;
int secret[MAX] = {0};
int guess[MAX] = {0};
int match[MAX] = {0};
//5 second timer
ulong timeout = 5000000;
int debug = 0;

/**
*program calls this function with a number and it will
*blink the green(yellow) led that number of times.
*/
int yblink(int n){
	
	int i;
	
	for (i = 0; i < n; i++){
		//yellowblink() is called from the header file blinkyellow.h.
		yellowblink();	
	
	}
	
	return 0;
}

/**
*program calls this function with a number and it will
*blink the red led that number of times.
*/
int rblink(int n){
	
	int i;
	
	for (i = 0; i < n; i++){
		//redblink() is called from the header file blinkred.h.
		redblink();	
	
	}
	
	return 0;
}

/**
*This function takes in the secret code from the codemaster by them clicking
*on the button a certain number of times.
*/
void inputSecret(){
	
	int i,k;
	
	//loops for the number of digits in the code.
	for(i=0; i < sizeof(secret)/sizeof(secret[0]); i++){

		//digit is set to the number of button presses in a certain time.
		secret[i] = inputbutton(timeout);

		//if codemaker enters a number above the maximum number (usually 3 but can be changed)
		//then the value will be set to the max number.
		if(secret[i]>MAXNUM)
			secret[i] = MAXNUM;

		//if the codemaker does not enter a value then it will default to 1.
		if(secret[i]==0)
			secret[i] = 1;

		//if debug mode is no print line to console (this statement used throughout program).
		if(debug == 1){
			
			printf("IN:   <Press %d> <Pause>\n",secret[i]);
			printf("OUT:  <Red Blink 1>\n");
		}
		
		//rblink used to call function to blink the red LED certain number of time 
		rblink(1);
		
		if(debug == 1){
		
		printf("OUT:  <Green Blink %d>\n",secret[i]);
		}

		//yblink used to call function to blink the green(yellow) LED certain number of time 
		yblink(secret[i]);
	}
	
	if(debug == 1){
		
		printf("OUT: \t\t\t\t Secret: ");
		
		for(k=0; k < sizeof(secret)/sizeof(secret[0]); k++){
			
			printf("%d ",secret[k]);
		}
		
		printf("\n");
	}
	
	if(debug == 1){
		
		printf("OUT:  <Red Blink 2>\n");
	}
	
	rblink(2);
}

/**
*This function takes in the guess code from the codebreaker that
*has been entered using the button.
*/
void inputGuess(){
	
	int i,k;
	
	//loops for the number of digits in the code.
	for(i=0; i < sizeof(guess)/sizeof(guess[0]); i++){
		
		//digit is set to the number of button presses in a certain time.
		guess[i] = inputbutton(timeout);

		//if codebreaker enters a number above the maximum number (usually 3 but can be changed)
		//then the value will be set to the max number.
		if(guess[i]>MAXNUM)
			guess[i] = MAXNUM;

		//if the codebreaker does not enter a value then it will default to 1.
		if(guess[i]==0)
			guess[i] = 1;

		//if debug mode is no print line to console (this statement used throughout program).
		if(debug == 1){
			printf("IN:   <Press %d> <Pause>\n",guess[i]);
			printf("OUT:  <Red Blink 1>\n");
		}
		
		rblink(1);
		
		if(debug == 1){
			printf("OUT:  <Green Blink %d>\n",guess[i]);
		}
		
		yblink(guess[i]);
	
	}
	
	if(debug == 1){
		
		printf("OUT:\t\t\t\t  Guess: ");
		
		for(k=0; k < sizeof(guess)/sizeof(guess[0]); k++){
			printf("%d ",guess[k]);
		}
		
		printf("\n");
	
	}
	if(debug == 1){
		printf("OUT:  <Red Blink 2>\n");
	}
	rblink(2);
}

/**
*This function finds all the exact matches between the
*guess code and the secret code.
*/
void exactFun(){
	
	int i;
	
	//loops for the number of digits in the code.
	for(i=0; i < sizeof(guess)/sizeof(guess[0]); i++){
		
		//if the digit in the guess code equal digit in the secret code
		//in the same position add one to the number of exacts
		if(guess[i]==secret[i]){
			
			exact++;
			//stores what number has been found.
			match[guess[i]-1] = -1;	
		}else{
			//checks if number already has had an exact or not 
			if(match[guess[i]-1] != -1){
				//if it hasnt had an exact before set it to be checked for approximates
				match[guess[i]-1] = guess[i];
			}
		}	
	}
	if(debug == 1){
		printf("OUT:  <Green Blink %d>\n",exact);
	}
	yblink(exact);
	if(debug == 1){
		printf("OUT:  <Red Blink 1>\n");
	}
	rblink(1);
}

/**
*This function finds all the approximate matches between the
*guess code and the secret code.
*/
void approximateFun(){
	
	int i,j;
	
	//loops for number of digits there are.
	for(i=0; i < sizeof(match)/sizeof(match[0]); i++){
		
		//if the digit has not had an exact already and is in in guess code
		if (match[i] != -1 && match[i] != 0){
			
			//loop for length of code
			for(j=0; j < sizeof(secret)/sizeof(secret[0]); j++){
				
				//if digit is found in the secret code add one to the number of approximates
				if ((i+1) == secret[j] && match[i] != -1){
					
					approximate++;
					match[i] = -1;
				}
			}
		}
	}
	
	if(debug == 1){
		
		printf("OUT:  <Green Blink %d>\n",approximate);
	}
	
	yblink(approximate);
}

/**
*This function is called when the secret code has been found
*by the codebreaker.
*/
void success(){
	
	char AttemptBuffer[20];
	
	if(debug == 1){
			
			printf("Codebreaker input stops here\n");
		}

		//sprintf sends formatted output of number of attempts to a string pointed to by AttemptBuffer
		sprintf(AttemptBuffer, "Attempts: %d",attempts);
		
		if(debug == 1){
			printf("OUT:  SUCCESS\n");
			printf("OUT:  Attempts: %d\n",attempts);
		}
		if(debug == 1){
			printf("OUT:  <Red On>\n");
		}
		//turns red LED on
		redon();
		
		if(debug == 1){
		
			printf("OUT:  <Green Blink 3>\n");
		}
		
		yblink(3);
		
		if(debug == 1){
			printf("OUT:  <Red Off>\n");
		}
		//turns red LED off
		redoff();
		
		//output success message and number of attempts to the LCD screen
		lcd("SUCCESS",AttemptBuffer);
		
		//waits for button to be presses and then clears the screen
		waitforpress();
		Clear();

}

/**
*main fuctionfor the program
*/
main (int argc, char *argv[]){

	char *first = "Press button";
	char *second = "to make code";
	char Ebuffer[20];
	char Abuffer[20];

//if there are 2 arguments and the second argument is -d then turn debug mode.
	if(argc == 2){
		
		if(strcmp(argv[1], "-d" )== 0){
			
			debug =1;
		}
	}

//lcd displays the message "Press button to make code" and 
//waits for the button to be presses to continue with the program.
	lcd(first,second);
	waitforpress();
	Clear();

	memset(secret,0,MAX);
	memset(guess,0,MAX);
	memset(match,0,MAX);
	
	if(debug == 1){
		
		printf("OUT:  <Red Blink 3>\n");
	}
	
	rblink(3);
	
	if(debug == 1){
		
		printf("Codemaker input starts here\n");
	}
	
	//calls inputSecret() so the codemaster can input a code to be broken.
	inputSecret();
	
	if(debug == 1){
		
		printf("Codemaker input stops here\n");
	}
	
	lcd("Press Button","to break code");
	
	//waits for button to be presses and then clears the screen
	waitforpress();
	Clear();
	
	if(debug == 1){
		
		printf("Codebreaker input starts here\n");
	}
	
	//loops while the number of exact values dosn not equal the number of digits
	while(exact != MAX){
		
		exact =0;
		approximate=0;
		
		if(debug == 1){
		
			printf("OUT:  <Red Blink 3>\n");
		}
		
		rblink(3);
		
		//calls inputGuess() so the codebreaker can input a code to try to find the secret code.
		inputGuess();
		
		//add one to the number of attempt it has taken
		attempts++;
		
		//calls the exactFun() to find the number of exact matches.
		exactFun();

		//if the number of exacts are not equal to the number of digits.
		if(exact != MAX){
			
			//call approximateFun() to find all the approximates.
			approximateFun();
		
		if(debug == 1){
			
			printf("OUT:  Exact: %d\n",exact);
			printf("OUT:  Approx: %d\n",approximate);
		}

		//formats the output of the number of exacts and approxes to a string
		sprintf(Ebuffer, "Exact: %d",exact);
		sprintf(Abuffer, "Approx: %d",approximate);

		//outputs the number of exacts and approximates to LCD screen.
		lcd(Ebuffer,Abuffer);
		
		//waits for button to be presses and then clears the screen
		waitforpress();
		Clear();
		}else{
		
			//calls the success() function to output the success message.
			success();
		}
	}
}