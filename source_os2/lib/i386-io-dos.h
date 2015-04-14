/*
 *	The PCI Library -- Access to i386 I/O ports on DOS
 *
 *	2006-05-31 Veit Kannegieser
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#include <conio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "search_ids.h"

#define outb(x,y) outp(y,x)
#define outw(x,y) outpw(y,x)
#define outl(x,y) outpd(y,x)
  
#define inb(x) inp(x)
#define inw(x) inpw(x)
#define inl(x) inpd(x)


static int
intel_setup_io(void)
{
  return 1;
}

static inline void
intel_cleanup_io(void)
{
}
