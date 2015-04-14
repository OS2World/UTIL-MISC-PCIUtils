/*
 *      The PCI Library -- DOS BIOS Int 1A access
 *
 *      ported 2005-12-07 Veit Kannegieser
 *
 *      Can be freely distributed and used under the terms of the GNU GPL.
 */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "pci.h"

#include "internal.h"
#include "search_ids.h"

#include <i86.h>
#include <stdbool.h>


static void
dos_bios_config(struct pci_access *a)
{
  locate_pci_ids(a);
}


static int
dos_bios_detect(struct pci_access *a)
{

  /* simply call the PCI BIOS install check function */
  union REGS regs;

  memset( &regs, 0, sizeof(union REGS) );
  regs.w.ax  = 0xb101;
  int386( 0x1a, &regs, &regs );

  if ((regs.h.ah != 0x00) | (regs.w.cflag && INTR_CF))
    {
      a->debug("...PCI BIOS is not implemented!");
      return 0;
    }

  if (regs.x.edx != 0x20494350) /* 'PCI ' */
    {
      a->debug("...PCI BIOS or DOS Extender register corruption!");
      return 0;
    }

  a->debug("...PCI BIOS %x.%x found.", regs.h.bh, regs.h.bl);

  return 1;
}


static void
dos_bios_init(struct pci_access *a)
{
  /* nothing to do! */
}


static void
dos_bios_cleanup(struct pci_access *a)
{
  /* nothing to do! */
}


static int
dos_bios_read(struct pci_dev *d, int pos, byte *buf, int len)
{
  union REGS regs;

  memset( &regs, 0, sizeof(union REGS) );
  regs.h.ah = 0xb1;
  regs.h.bh = d->bus;
  regs.h.bl = PCI_DEVFN(d->dev, d->func);
  regs.w.di = pos;

  switch (len)
    {
    case 1:
      regs.h.al = 0x08;
      int386( 0x1a, &regs, &regs );
               buf [0] = regs.h.cl;
      break;
    case 2:
      regs.h.al = 0x09;
      int386( 0x1a, &regs, &regs );
      ((u16 *) buf)[0] = regs.w.cx;
      break;
    case 4:
      regs.h.al = 0x0a;
      int386( 0x1a, &regs, &regs );
      ((u32 *) buf)[0] = regs.x.ecx;
      break;
    default:
      return pci_generic_block_read(d, pos, buf, len);
    }

  if ((regs.h.ah != 0x00) | (regs.w.cflag && INTR_CF))
    return 0;

  return 1;
}

static int
dos_bios_write(struct pci_dev *d, int pos, byte *buf, int len)
{
  union REGS regs;

  memset( &regs, 0, sizeof(union REGS) );
  regs.h.ah = 0xb1;
  regs.h.bh = d->bus;
  regs.h.bl = PCI_DEVFN(d->dev, d->func);
  regs.w.di = pos;

  switch (len)
    {
    case 1:
      regs.h.al  = 0x0b;
      regs.h.cl  =          buf [0];
      int386( 0x1a, &regs, &regs );
      break;
    case 2:
      regs.h.al  = 0x0c;
      regs.w.cx  = ((u16 *) buf)[0];
      int386( 0x1a, &regs, &regs );
      break;
    case 4:
      regs.h.al  = 0x0d;
      regs.x.ecx = ((u32 *) buf)[0];
      int386( 0x1a, &regs, &regs );
      break;
    default:
      return pci_generic_block_write(d, pos, buf, len);
    }

  if ((regs.h.ah != 0x00) | (regs.w.cflag && INTR_CF))
    return 0;

  return 1;
}

struct pci_methods pm_dos_bios = {
  "DOS-BIOS-INT1A",
  dos_bios_config,
  dos_bios_detect,
  dos_bios_init,
  dos_bios_cleanup,
  pci_generic_scan,
  pci_generic_fill_info,
  dos_bios_read,
  dos_bios_write,
  NULL,                                 /* dev_init */
  NULL                                  /* dev_cleanup */
};

