#!/bin/bash
# File:    test.sh
# Date:    April 2018
# Author:  Klara Necasova
# Email:   xnecas24@stud.fit.vutbr.cz
# Project: Implementation of preorder order assignment to vertexes - project 3 for PRL 

# checks number of arguments
if [ $# -ne 1 ]; then
   echo "invalid number of arguments"
   exit 1
fi

inputLength=${#1}
processes=$((2 * inputLength - 2))

if [ "${processes}" -le "0" ]; then
	processes=1
fi

# compiles
mpic++ --prefix /usr/local/share/OpenMPI -o pro pro.cpp

# runs
mpirun --prefix /usr/local/share/OpenMPI -np $processes pro "${1}"

# removes created files
rm -f pro
