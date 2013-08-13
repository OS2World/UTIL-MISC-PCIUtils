@echo off
rem
rem download and unpack pci.ids database
rem

wget --output-document=pci.ids.bz2 http://pciids.sourceforge.net/v2.2/pci.ids.bz2

if not exist pci.ids.bz2 goto end

bzip2 -d < pci.ids.bz2 > pci.ids

:end
