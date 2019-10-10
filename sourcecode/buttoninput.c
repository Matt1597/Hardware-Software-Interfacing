#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#include <string.h> 
#include <signal.h>
#include <signal.h>
#include <stdint.h>

#include <unistd.h>

#include <sys/ioctl.h>

#define ACT  5

// delay for each button press
#define DELAY 400

#define LED 6
#define BUTTON 19


#ifndef	TRUE
#define	TRUE	(1==1)
#define	FALSE	(1==2)
#endif

#define	PAGE_SIZE		(4*1024)
#define	BLOCK_SIZE		(4*1024)

#define	INPUT			 0
#define	OUTPUT			 1

#define	LOW			 0
#define	HIGH			 1

//The button waits for input for 5 seconds
#define TIMEOUT 5000000


static volatile unsigned int gpiobase ;
static volatile uint32_t *gpio ;

static volatile unsigned int timerbase ;
static volatile uint32_t *timer ;

static uint64_t startT, stopT;


//this function records the user input for the number and responds by turning on the yellow/green LED
//everytime the button is pressed
int inputbutton(ulong Long)
{
  int fd;
  fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC);
  gpiobase = 0x3F200000 ;

  timerbase = (unsigned int)0x3F003000 ;

  gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, gpiobase) ;

  timer = (int32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, timerbase) ;

//flushes the output stream bugger of stderr

  fflush(stderr);
  
  volatile uint32_t ts = *(timer+1); // word offset
  uint32_t n=0, m=0;
  uint32_t last, curr;

//counter used to return how many times the button was pressed to the main function Mastermind.c
  int counter;
  counter = 0;

#if defined(DEBUG)
  fprintf (stderr, "Waiting for %d micro-seconds (reading counter value from %x)\n", Long, (timer+1));
  last = *(timer+1); 
#endif

  int pinACT = ACT,  pinLED = LED, pinButton = BUTTON;
  int fSel,fSelB, shiftR, shiftY, shiftB, pin, clrOff, setOff;
  int theValue;
  unsigned int howLong = DELAY;
  uint32_t res; /* testing only */

  // setting the mode
  fSel =  0;    // GPIO 5 lives in register 0 (GPFSEL)  
  shiftY =  18;  // GPIO 6 sits in slot 6 of register 0, thus shift by 6*3 (3 bits per pin)

  // controlling button pin 19
  fSelB =  1;    // GPIO 19 lives in register 1 (GPFSEL1)
  shiftB =  27;  // GPIO 19 sits in slot 6 of register 1, thus shift by 9*3 (3 bits per pin)

//assembler is used to set the mode of the yellow LED

  asm(/* inline assembler version of setting yellow LED to ouput" */
      "\tB _yellowinput\n"
      "_yellowinput: NOP\n"
      "\tLDR R1, %[gpio]\n"
      "\tADD R0, R1, %[fSel]\n"  /* R0 = GPFSEL register to write to */
      "\tLDR R1, [R0, #0]\n"     /* read current value of the register */
      "\tMOV R2, #0b111\n"
      "\tLSL R2, %[shiftY]\n"
      "\tBIC R1, R1, R2\n"
      "\tMOV R2, #1\n"
      "\tLSL R2, %[shiftY]\n"
      "\tORR R1, R2\n"
      "\tSTR R1, [R0, #0]\n"
      "\tMOV %[result], R1\n"
      : [result] "=r" (res)
      : [yellow] "r" (pinLED)
      , [gpio] "m" (gpio)
      , [fSel] "r" (fSel*4)  /* offset in bytes! */
      , [shiftY] "r" (shiftY)
      : "r0", "r1", "r2", "cc");
      
      
//assembler is used to set the mode of the button

      asm(/* inline assembler version of setting button to input */
      "\tB _button\n"
      "_button: NOP\n"
      "\tLDR R1, %[gpio]\n"
      "\tADD R0, R1, %[fSelB]\n"  /* R0 = GPFSEL register to write to */
      "\tLDR R1, [R0, #0]\n"     /* read current value of the register */
      "\tMOV R2, #0b111\n"
      "\tLSL R2, %[shiftB]\n"
      "\tBIC R1, R1, R2\n"
      "\tMOV R2, #0\n"
      "\tLSL R2, %[shiftB]\n"
      "\tORR R1, R2\n"
      "\tSTR R1, [R0, #0]\n"
      "\tMOV %[result], R1\n"
      : [result] "=r" (res)
      : [but] "r" (pinButton)
      , [gpio] "m" (gpio)
      , [fSelB] "r" (fSelB*4)  /* offset in bytes! */
      , [shiftB] "r" (shiftB)
      : "r0", "r1", "r2", "cc");
     


//while loop checks for input for 5 seconds

while( ( (curr=*(timer+1)) - ts ) < Long ) {
	
 
    	theValue = HIGH ;

    	if ((*(gpio + 13 /* GPLEV0 */) & (1 << (BUTTON & 31))) != 0)
	{
	  theValue = HIGH ;
	  counter++;
	  
	}
        else
	{
          theValue = LOW ;
	}
      
   
    if ((pinButton & 0xFFFFFFC0 /* PI_GPIO_MASK */) == 0)	// bottom 64 pins belong to the Pi	
       {
	int off = (theValue == LOW) ? 10 : 7; // LED 5 and 6; register number for GPSET/GPCLR
        uint32_t res;

//assembler is used to turn on button if it's pressed by the user      
   
	asm volatile(/* inline assasm volatile(/* inline assembler version of setting/clearing LED to ouput" */
                "\tB   _yellowON\n"
                "_yellowON: NOP\n"
                "\tLDR R1, %[gpio]\n"
                "\tADD R0, R1, %[off]\n"  /* R0 = GPSET/GPCLR register to write to */
                "\tMOV R2, #1\n"
                "\tMOV R1, %[yellow]\n"
                "\tAND R1, #31\n"
                "\tLSL R2, R1\n"          /* R2 = bitmask setting/clearing register %[act] */
                "\tSTR R2, [R0, #0]\n"    /* write bitmask */
                "\tMOV %[result], R2\n"
                : [result] "=r" (res)
                : [yellow] "r" (pinLED)
                , [gpio] "m" (gpio)
                , [off] "r" (off*4)
                : "r0", "r1", "r2", "cc");



	}
//There is a 400ms delay between each button press so that only one press is recorded
//everytime the button is pressed
	
    {
      struct timespec sleeper, dummy ;

      sleeper.tv_sec  = (time_t)(howLong / 1000) ;
      sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

      nanosleep (&sleeper, &dummy) ;
    }
  



}
 // Clean up: write LOW

 clrOff = 10; // 
 *(gpio + clrOff) = 1 << (pinLED & 31) ;

 
return counter;
}




