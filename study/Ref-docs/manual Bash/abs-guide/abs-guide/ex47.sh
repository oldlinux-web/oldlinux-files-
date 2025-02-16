#!/bin/bash
# printf demo

PI=3.14159265358979
DecimalConstant=31373
Message1="Greetings,"
Message2="Earthling."

echo

printf "Pi to 2 decimal places = %1.2f" $PI
echo
printf "Pi to 9 decimal places = %1.9f" $PI  # It even rounds off correctly.

printf "\n"                                  # Prints a line feed,
                                             # equivalent to 'echo'.

printf "Constant = \t%d\n" $DecimalConstant  # Inserts tab (\t)

printf "%s %s \n" $Message1 $Message2

echo

# ==========================================#
# Simulation of C function, 'sprintf'.
# Loading a variable with a formatted string.

echo 

Pi12=$(printf "%1.12f" $PI)
echo "Pi to 12 decimal places = $Pi12"

Msg=`printf "%s %s \n" $Message1 $Message2`
echo $Msg; echo $Msg

# As it happens, the 'sprintf' function can now be accessed
# as a loadable module to Bash, but this is not portable.

exit 0
