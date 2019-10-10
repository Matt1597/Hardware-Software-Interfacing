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




static volatile unsigned int gpiobase ;
static volatile uint32_t *gpio ;

//function waits for user to press button. It's used to have a pause between rounds so that the next function
//doesn't run straight away

int waitforpress()
{
  int fd;
  fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC);
  gpiobase = 0x3F200000 ;



  gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, gpiobase) ;

 
  int pinButton = BUTTON;
  int fSel,fSelB, shiftB, pin, clrOff, setOff;
  int theValue;

  uint32_t res; /* testing only */



  // controlling button pin 19
  fSelB =  1;    // GPIO 19 lives in register 1 (GPFSEL1)
  shiftB =  27;  // GPIO 19 sits in slot 9 of register 1, thus shift by 9*3 (3 bits per pin)


//inline assembler is used to set the mode of the button as an input


      asm(/* inline assembler version of setting red LED to ouput" */
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
     
int pressed = 0;

//while loop runs until the button is pressed
while(pressed == 0 ) {
	
 
    	

    	if ((*(gpio + 13 /* GPLEV0 */) & (1 << (BUTTON & 31))) != 0)
	{
	    pressed =1;
	  
	}
      
      
   

	
  
  



}

 
return pressed;
}




