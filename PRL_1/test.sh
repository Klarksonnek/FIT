#!/bin/bash

# File:    test.sh
# Date:    April 2018
# Author:  Klara Necasova
# Email:   xnecas24@stud.fit.vutbr.cz
# Project: Implementation of "Merge-splitting sort" algorithm - project 2 for PRL 

# checks number of arguments
if [ $# -ne 2 ]; then
   echo "invalid number of arguments"
   exit 1
fi

numberCount=$1
numberProc=$2

if [ $numberCount -lt 1 ]; then
   echo "invalid number of numbers, it must be greater than zero"
   exit 1
fi

if [ $numberProc -lt 1 ]; then
   echo "invalid number of processors, it must be greater than zero"
   exit 1
fi

# compiles
mpic++ --prefix /usr/local/share/OpenMPI -o mss mss.cpp
            
# creates file containing random numbers
dd if=/dev/random bs=1 count=$numberCount of=numbers 2>/dev/null

# runs
mpirun --prefix /usr/local/share/OpenMPI -np $numberProc mss

# removes created files
rm -f mss numbers