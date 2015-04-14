/*
 *      The PCI Library -- OS/2 \dev\oemhlp$ driver access
 *
 *      ported 2005-11-08 Veit Kannegieser
 *
 *      Can be freely distributed and used under the terms of the GNU GPL.
 */

#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSERRORS
#define INCL_DOSMISC
#define INCL_DOSMODULEMGR

#include <os2.h>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "pci.h"

#include "internal.h"
#include "search_ids.h"

#include "os2-device.h"


static void
os2_oemhlp_config(struct pci_access *a)
{

  a->method_params[PCI_ACCESS_OS2_OEMHLP] = PCI_PATH_OS2_DEVICE;

  locate_pci_ids(a);
}


static int
os2_oemhlp_detect(struct pci_access *a)
{
  char *name = a->method_params[PCI_ACCESS_OS2_OEMHLP];

  a->debug("...using %s", name);
  return 1;
}


static void
os2_oemhlp_init(struct pci_access *a)
{
  char *name = a->method_params[PCI_ACCESS_OS2_OEMHLP];

  a->fd = open(name, O_RDWR, 0);
  if (a->fd < 0)
    a->error("os2_oemhlp_init: %s open failed", name);

}


static void
os2_oemhlp_cleanup(struct pci_access *a)
{
  close(a->fd);
}


static int
os2_oemhlp_read(struct pci_dev *d, int pos, byte *buf, int len)
{
  ULONG                                                 rc;
  struct Parameter_Packet_Read_PCI_Configuration_Space  ParameterPacket;
  struct Data_Packet_Read_PCI_Configuration_Space       DataPacket;
  ULONG                                                 ParameterPacketLen;
  ULONG                                                 DataPacketLen;

  if (!(len == 1 || len == 2 || len == 4))
    return pci_generic_block_read(d, pos, buf, len);

  if (pos+len > 0x100)
    return 0;

  ParameterPacket.Subfunction_Number = OEMHLP_PCI_Subfunction_Read;
  ParameterPacket.Bus_Number = d->bus;
  ParameterPacket.DevFuncNum = PCI_DEVFN(d->dev, d->func);
  ParameterPacket.Configuration_Register = pos;
  ParameterPacket.Size = len;

  ParameterPacketLen = sizeof(ParameterPacket);
  DataPacketLen      = sizeof(DataPacket);

  rc = DosDevIOCtl(d->access->fd, IOCTL_OEMHLP, OEMHLP_PCI,
                   &ParameterPacket, ParameterPacketLen, &ParameterPacketLen,
                   &DataPacket     , DataPacketLen     , &DataPacketLen     );

  if (rc == IOCtlError_Invalid_Parameter)
    {
      /* Invalid Parameter -> guess non-existing device */
      DataPacket.Data = 0xffffffff;
      DataPacket.Return_Code = 0;
      rc = NO_ERROR;
    }

  if (rc != NO_ERROR)
    d->access->error("os2_oemhlp_read: DosDevIOCtl failed, error=%d", rc);

  if (DataPacket.Return_Code != NO_ERROR)
    d->access->error("os2_oemhlp_read: DosDevIOCtl Return Code %d", DataPacket.Return_Code);

  switch (len)
    {
    case 1:
               buf [0] =  (u8) DataPacket.Data;
      break;
    case 2:
      ((u16 *) buf)[0] = (u16) DataPacket.Data;
      break;
    case 4:
      ((u32 *) buf)[0] = (u32) DataPacket.Data;
      break;
    }
  return 1;
}

static int
os2_oemhlp_write(struct pci_dev *d, int pos, byte *buf, int len)
{
  ULONG                                                 rc;
  struct Parameter_Packet_Write_PCI_Configuration_Space ParameterPacket;
  struct Data_Packet_Write_PCI_Configuration_Space      DataPacket;
  ULONG                                                 ParameterPacketLen;
  ULONG                                                 DataPacketLen;

  if (!(len == 1 || len == 2 || len == 4))
    return pci_generic_block_write(d, pos, buf, len);

  if (pos+len > 0x100)
    return 0;

  ParameterPacket.Subfunction_Number = OEMHLP_PCI_Subfunction_Write;
  ParameterPacket.Bus_Number = d->bus;
  ParameterPacket.DevFuncNum = PCI_DEVFN(d->dev, d->func);
  ParameterPacket.Configuration_Register = pos;
  ParameterPacket.Size = len;

  switch (len)
    {
    case 1:
      ParameterPacket.Data =          buf [0];
      break;
    case 2:
      ParameterPacket.Data = ((u16 *) buf)[0];
      break;
    case 4:
      ParameterPacket.Data = ((u32 *) buf)[0];
      break;
    }

  ParameterPacketLen = sizeof(ParameterPacket);
  DataPacketLen      = sizeof(DataPacket);

  rc = DosDevIOCtl(d->access->fd, IOCTL_OEMHLP, OEMHLP_PCI,
                   &ParameterPacket, ParameterPacketLen, &ParameterPacketLen,
                   &DataPacket     , DataPacketLen     , &DataPacketLen     );

  if (rc != NO_ERROR)
    d->access->error("os2_oemhlp_write: DosDevIOCtl failed, error=%d", rc);

  if (DataPacket.Return_Code != NO_ERROR)
    d->access->error("os2_oemhlp_write: DosDevIOCtl Return Code %d", DataPacket.Return_Code);

  return 1;
}

struct pci_methods pm_os2_oemhlp = {
  "OS/2-OEMHLP-driver",
  os2_oemhlp_config,
  os2_oemhlp_detect,
  os2_oemhlp_init,
  os2_oemhlp_cleanup,
  pci_generic_scan,
  pci_generic_fill_info,
  os2_oemhlp_read,
  os2_oemhlp_write,
  NULL,                                 /* dev_init */
  NULL                                  /* dev_cleanup */
};

