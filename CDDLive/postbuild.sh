#!/bin/bash

echo === post build start ===
if [ $# -lt 4 ]
then
  echo "Example Usage :  postbuild.sh <Model> <Objcopy_exe>         <ProjName> <ProjDirPath>"
  echo "            e.g. postbuild.sh CDDL_12 arm-none-eabi-objcopy  CDDLive   C:/PATH_to_Project/CDDLive"
  exit 1
fi

MODEL=$1
OBJCOPY=$2
PROJ=$3
PROJPATH=$4

# Extract the Firmware Version automatically from oly.h into variable 'VERSION'
VERSION=""
olyHeaderFile=$PROJPATH/Common/oly.h
found=0
while read -r line
do
    case $line in
        *"OLY_FW_VERSION"*)
            found=$((found+1))
            if [ $found -eq 2 ]; then
                echo Using Version information from Oly.h line : "$line"
                VERSION=$(echo "$line | "tr '\n' ' ' | sed -e 's/[^0-9]/ /g' -e 's/^ *//g' -e 's/ *$//g' | tr ' ' '.' | sed 's/ /\n/g') ;
            fi
            ;;
    esac
done < "$olyHeaderFile"


IMGNAME="OLY_APPLICATION_${VERSION}_${MODEL}_NU.img"
BINNAME="OLY_APPLICATION_${VERSION}_${MODEL}_NU.bin"
HEXNAME="OLY_APPLICATION_${VERSION}_${MODEL}_NU.hex"
BOOTAPPNAME="BOOT_APP_${VERSION}_${MODEL}_NU"


echo ==========================================================================================================================
echo Running postbuild.sh with parameters : $1 $OBJCOPY  $PROJ $PROJPATH
echo
echo NOTE : VERSION $VERSION will be applied, extracted from oly.h
echo
echo ==========================================================================================================================

echo Creating $BINNAME
$OBJCOPY -O binary "$PROJ.elf" $BINNAME

echo Creating $HEXNAME
$OBJCOPY -O ihex "$PROJ.elf" $HEXNAME



if ! command -v srec_cat.exe &> /dev/null
then
    echo "srec_cat.exe not found."
    echo "    Please install from : https://sourceforge.net/projects/srecord/files/srecord-win32/1.65/"  
    echo "    and add to PATH environment variable."  
    exit
else
    echo Creating "$BOOTAPPNAME.bin"
    srec_cat.exe "$PROJPATH/../BOOTLOADER/BOOTLOADER.hex" -Intel $HEXNAME -Intel -o "$BOOTAPPNAME.bin" -binary
fi
# If above doesn't work, then add the full path to wherever you have installed srec_cat.exe on your system. e.g. 
# "C:\Program Files\srecord\bin\srec_cat.exe" "$PROJPATH/../BOOTLOADER/BOOTLOADER.hex" -Intel $HEXNAME -Intel -o "$BOOTAPPNAME.bin" -binary



if [ -e $BINNAME ]
then
    echo Creating $IMGNAME
    "$PROJPATH/../TOOLS/OlyPack.exe" -L $PROJ.log -I $BINNAME -O $IMGNAME -V $VERSION -S 0x4000 -T 0x4000 -P 0x28000
else
    echo WARNING : $BINNAME was not created
fi

echo === post build end ===