/*
 *      The PCI Library -- Access to i386 I/O ports on OS/2
 *
 *      2006-05-31 Veit Kannegieser
 *
 *      Can be freely distributed and used under the terms of the GNU GPL.
 */


#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "search_ids.h"

#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS
#include <os2.h>


/* we must call IO code in an IOPL segment, usually not 32 bit ring 3 code,
 * so <conio.h> functions will not help.
 *
 * possible alternatives
 * - link an thunk object (like Virtual Pascal) 32 -> 16:16 -> 16:16 IOPL
 * - IOCTL to device drivers (/dev/testcfg$ or xfree86.sys)
 * - use a DLL
 *
 * we choose the third method, using EMXIO.DLL
 */

HMODULE emxio_dll = 0;
BOOL    unload_emxio_dll = FALSE;

typedef unsigned int      (APIENTRY TINP8 ) (int port);
typedef unsigned int      (APIENTRY TINP16) (int port);
typedef unsigned long int (APIENTRY TINP32) (int port);
typedef void      	  (APIENTRY TOUT8 ) (int port, unsigned int      value);
typedef void      	  (APIENTRY TOUT16) (int port, unsigned int      value);
typedef void	          (APIENTRY TOUT32) (int port, unsigned long int value);

TINP8  *EMX_INP8  ;
TINP16 *EMX_INP16 ;
TINP32 *EMX_INP32 ;
TOUT8  *EMX_OUTP8 ;
TOUT16 *EMX_OUTP16;
TOUT32 *EMX_OUTP32;


#define outb(x,y) EMX_OUTP8(y,x)
#define outw(x,y) EMX_OUTP16(y,x)
#define outl(x,y) EMX_OUTP32(y,x)

#define inb(x) EMX_INP8(x)
#define inw(x) EMX_INP16(x)
#define inl(x) EMX_INP32(x)


static int
intel_setup_io(void)
{
  int rc;
  UCHAR LoadError[256] = "";

  rc = DosQueryModuleHandle("EMXIO.DLL", &emxio_dll);
  if (rc != NO_ERROR)
    rc = DosLoadModule(LoadError, sizeof(LoadError), "EMXIO.DLL", &emxio_dll);

  if (rc != NO_ERROR)
    return 0;
    
  unload_emxio_dll = TRUE;    

  if ( (DosQueryProcAddr(emxio_dll, 1, NULL, (PFN *) &EMX_INP8  ) != NO_ERROR)
     | (DosQueryProcAddr(emxio_dll, 2, NULL, (PFN *) &EMX_OUTP8 ) != NO_ERROR)
     | (DosQueryProcAddr(emxio_dll, 3, NULL, (PFN *) &EMX_INP16 ) != NO_ERROR)
     | (DosQueryProcAddr(emxio_dll, 4, NULL, (PFN *) &EMX_OUTP16) != NO_ERROR)
     | (DosQueryProcAddr(emxio_dll, 5, NULL, (PFN *) &EMX_INP32 ) != NO_ERROR)
     | (DosQueryProcAddr(emxio_dll, 6, NULL, (PFN *) &EMX_OUTP32) != NO_ERROR) )
    return 0;

  return 1;
}

static inline void
intel_cleanup_io(void)
{
  if (unload_emxio_dll)
    {
      unload_emxio_dll = FALSE;
      DosFreeModule(emxio_dll);
      emxio_dll = 0;
    }
}
