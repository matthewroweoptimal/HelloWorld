#!/bin/bash

echo === pre build start ===

if [ $# -ne 1 ]
then
  echo "Example Usage :  prebuild.sh <CWD>"
  echo "            e.g. prebuild.sh C:/PATH_to_Project/CDDLive/Release"
  exit 1
fi

OUTPUTDIR=$1

echo Deleting "$OUTPUTDIR"/*.hex
rm "$OUTPUTDIR"/*.hex

echo Deleting "$OUTPUTDIR"/*.bin
rm "$OUTPUTDIR"/*.bin

echo Deleting "$OUTPUTDIR"/*.img
rm "$OUTPUTDIR"/*.img


echo === pre build end ===