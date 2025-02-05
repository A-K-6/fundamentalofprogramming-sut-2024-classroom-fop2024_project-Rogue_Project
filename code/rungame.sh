#!/bin/bash

# Install required libraries
sudo apt-get update
sudo apt-get install libncurses5-dev libmenu-dev libform-dev libsqlite3-dev

# Compile the program
gcc main.c -lncurses -lmenu -lform -lsqlite3 -o main

# Run the compiled program
./main

# gcc main.c -lncurses -lmenu -lform -lsqlite3 -o main 

# ./main

# sudo apt-get install libsqlite3-dev 
#  -lsqlite3
