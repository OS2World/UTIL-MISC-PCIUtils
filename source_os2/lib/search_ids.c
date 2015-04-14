/*
 *      The PCI Library -- locate file pci.ids
 *
 *      2006-05-31 Veit Kannegieser
 *
 *      Can be freely distributed and used under the terms of the GNU GPL.
 */

#include "internal.h"
#include "search_ids.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char pci_ids_full_path[260] = "";

#ifdef PCI_OS_OS2

#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSERRORS
#define INCL_DOSMISC
#define INCL_DOSMODULEMGR

#include <os2.h>


void locate_pci_ids(struct pci_access *a)
  {
  BOOL          found_pci_ids;
  char          *unixroot;
  char          *append_position;
  HMODULE       hMod;
  ULONG         ObjNum;
  ULONG         Offset;


  /*printf("Searching pci.ids..");*/

  /* platform specific: search id_file_name.. */

  /* 0) look if it already has been correctly defined.. */

  strcpy(pci_ids_full_path, a->id_file_name);
  found_pci_ids = (access(pci_ids_full_path, R_OK) == 0);


  /* 1) %UNIXROOT%/usr/share/ */

  if (!found_pci_ids)
    {
      unixroot = getenv("UNIXROOT");
      if (unixroot != NULL)
        {
          strcpy(pci_ids_full_path, unixroot);
          strcat(pci_ids_full_path, "\\usr\\share\\pci.ids");
          found_pci_ids = (access(pci_ids_full_path, R_OK) == 0);
        }
    }


  /* 2) own executable directory */

  if (!found_pci_ids)
    {
      if (DosQueryModFromEIP(&hMod, &ObjNum, sizeof(pci_ids_full_path), (PCHAR) &pci_ids_full_path, &Offset, (ULONG)&locate_pci_ids ) == NO_ERROR)
        {
          if (DosQueryModuleName(hMod, sizeof(pci_ids_full_path), (PCHAR) &pci_ids_full_path) == NO_ERROR)
            {
              append_position = strrchr(pci_ids_full_path, '\\');
              if (append_position != NULL)
                {
                  strcpy(append_position, "\\pci.ids");
                  found_pci_ids = (access(pci_ids_full_path, R_OK) == 0);

                  /* find pci.ids when we are os2\lspci.exe.. */
                  if (!found_pci_ids)
                    {
                      strcpy(append_position, "\\..\\pci.ids");
                      found_pci_ids = (access(pci_ids_full_path, R_OK) == 0);
                    }
                }
            }
        }
    }


  /* 3) %DPATH%, current directory first */

  if (!found_pci_ids)
    {
      if (DosSearchPath(1*SEARCH_IGNORENETERRS
                      +1*SEARCH_ENVIRONMENT
                      +1*SEARCH_CUR_DIRECTORY,
                       "DPATH",
                       PCI_PATH_IDS, /* pci.ids */
                       pci_ids_full_path,
                       sizeof(pci_ids_full_path))  == NO_ERROR)
        {
          found_pci_ids = (access(pci_ids_full_path, R_OK) == 0);
        }
    }

  if (found_pci_ids)
    {
      a->id_file_name = pci_ids_full_path;
    }

  /*printf(" '%s'\n", a->id_file_name);*/

  }

#endif


#ifdef PCI_OS_DOS


void locate_pci_ids(struct pci_access *a)
  {
  bool          found_pci_ids;
  char          *append_position;

  /*printf("Searching pci.ids..");*/

  /* platform specific: search id_file_name.. */

  /* 0) look if it already has been correctly defined.. */

  strcpy(pci_ids_full_path, a->id_file_name);
  found_pci_ids = (access(pci_ids_full_path, R_OK) == 0);


  /* 1) own executable directory */

  if (!found_pci_ids)
    {
      strcpy(&pci_ids_full_path, *__argv);
      append_position = strrchr(pci_ids_full_path, '\\');
      if (append_position != NULL)
        {
          strcpy(append_position, "\\pci.ids");
          found_pci_ids = (access(pci_ids_full_path, R_OK) == 0);

          /* find pci.ids when we are dos\lspci.exe.. */
          if (!found_pci_ids)
            {
              strcpy(append_position, "\\..\\pci.ids");
              found_pci_ids = (access(pci_ids_full_path, R_OK) == 0);
            }
        }
    }

  /* 2) hope that the user has APPEND... */
  if (!found_pci_ids)
    strcpy(append_position, "pci.ids");


  if (found_pci_ids)
    {
      a->id_file_name = pci_ids_full_path;
    }

  /*printf(" '%s'\n", a->id_file_name);*/

  }

#endif
