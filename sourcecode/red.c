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


// =======================================================
// Tunables
// PINs (based on BCM numbering)

#define LED5 5



// =======================================================

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


#define	PI_GPIO_MASK	(0xFFFFFFC0)



//function to turn off red LED

void redoff ()
{
  int pinLED5 = LED5;
  int fSel, shift, pin,  clrOff, setOff, off;
  int   fd ;
  int   j;
  int theValue, thePin;

  uint32_t res; /* testing only */



  if (geteuid () != 0)
    fprintf (stderr, "setup: Must be root. (Did you forget sudo?)\n") ;

  // -----------------------------------------------------------------------------
  // constants for RPi2
  gpiobase = 0x3F200000 ;

  // -----------------------------------------------------------------------------
  // memory mapping 
  // Open the master /dev/memory device

  fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC);


  // GPIO:
  gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, gpiobase) ;


  // -----------------------------------------------------------------------------
  // setting the mode

  // controlling LED pin 6
 

  // controlling LED pin 5
  fSel =  0;    // GPIO 5 lives in register 0 (GPFSEL0)
  shift =  15;  // GPIO 5 sits in slot 5 of register 0, thus shift by 5*3 (3 bits per pin)
  // C version of setting LED
  *(gpio + fSel) =(*(gpio + fSel) & ~(7 << shift)) | (1 <<shift) ;  // Sets bits to one = output

  

  // ----------------------------------------------------------------------------

clrOff = 10; // GPCLR0 for pin 12
*(gpio + clrOff) = 1 << (LED5 & 31) ;

    
}

//function to turn on red LED

void redon ()
{
  int pinLED5 = LED5;
  int fSel, shift, pin,  clrOff, setOff, off;
  int   fd ;
  int   j;
  int theValue, thePin;

  uint32_t res; /* testing only */



  if (geteuid () != 0)
    fprintf (stderr, "setup: Must be root. (Did you forget sudo?)\n") ;

  // -----------------------------------------------------------------------------
  // constants for RPi2
  gpiobase = 0x3F200000 ;

  // -----------------------------------------------------------------------------
  // memory mapping 
  // Open the master /dev/memory device

  fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC);
    

  // GPIO:
  gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, gpiobase) ;


  // -----------------------------------------------------------------------------
  // setting the mode

  // controlling LED pin 6
  

  // controlling LED pin 5
  fSel =  0;    // GPIO 5 lives in register 0 (GPFSEL0)
  shift =  15;  // GPIO 5 sits in slot 5 of register 0, thus shift by 5*3 (3 bits per pin)
  // C version of setting LED
  *(gpio + fSel) =(*(gpio + fSel) & ~(7 << shift)) | (1 <<shift) ;  // Sets bits to one = output

  

  // -----------------------------------------------------------------------------
setOff = 7; // GPSET0 for pin 12
*(gpio + setOff) = 1 << (LED5 & 31) ;

	
}