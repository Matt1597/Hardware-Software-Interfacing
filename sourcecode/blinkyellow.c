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
// The OK/Act LED is connected to BCM_GPIO pin 12 (RPi 2)
#define ACT  6
// delay for blinking
#define DELAY 400
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

//This function blinks the yellow LED once

void yellowblink()
{
  int pinACT = ACT; 
  int fSel, shift, pin, clrOff, setOff;
  int   fd ;
  int   j;
  int theValue, thePin;
  unsigned int howLong = DELAY;
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
  fSel =  0;    // GPIO 12 lives in register 1 (GPFSEL)
  shift =  18;  // GPIO 12 sits in slot 2 of register 1, thus shift by 6*3 (3 bits per pin)
  *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) | (1 << shift) ;  // Sets bits to one = output

  // -----------------------------------------------------------------------------


//delays are added so that the LEDs do not blink too quickly
      {
        struct timespec sleeper, dummy ;
        sleeper.tv_sec  = (time_t)(howLong / 1000) ;
        sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

        nanosleep (&sleeper, &dummy) ;
      } 
  
    theValue = ((j % 2) == 0) ? HIGH : LOW;

    if ((pinACT & 0xFFFFFFC0 /* PI_GPIO_MASK */) == 0)	// bottom 64 pins belong to the Pi	
      {
	int off = (theValue == LOW) ? 10 : 7; // ACT/LED 47; register number for GPSET/GPCLR
        *(gpio + off) = 1 << (pinACT & 31) ;
      }
      else
      {
        fprintf(stderr, "only supporting on-board pins\n");          exit(1);
       }

//another delay before LED is turned off
       {
      struct timespec sleeper, dummy ;

      // fprintf(stderr, "delaying by %d ms ...\n", howLong);
      sleeper.tv_sec  = (time_t)(howLong / 1000) ;
      sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

      nanosleep (&sleeper, &dummy) ;
    }
   
  
 // Clean up: write LOW
 clrOff = 10; 
 *(gpio + clrOff) = 1 << (pinACT & 31) ;

}
