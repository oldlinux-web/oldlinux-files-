#!/bin/bash
# numbers.sh: Representation of numbers.

# Decimal
let "dec = 32"
echo "decimal number = $dec"             # 32
# Nothing out of the ordinary here.


# Octal: numbers preceded by '0' (zero)
let "oct = 071"
echo "octal number = $oct"               # 57
# Expresses result in decimal.

# Hexadecimal: numbers preceded by '0x' or '0X'
let "hex = 0x7a"
echo "hexadecimal number = $hex"         # 122
# Expresses result in decimal.

# Other bases: BASE#NUMBER
# BASE between 2 and 64.

let "bin = 2#111100111001101"
echo "binary number = $bin"              # 31181

let "b32 = 32#77"
echo "base-32 number = $b32"             # 231

let "b64 = 64#@_"
echo "base-64 number = $b64"             # 4094
#
# This notation only works for a limited range (2 - 64)
# 10 digits + 26 lowercase characters + 26 uppercase characters + @ + _

echo

echo $((36#zz)) $((2#10101010)) $((16#AF16)) $((53#1aA))
                                         # 1295 170 44822 3375


# Important note:
#  Using a digit out of range of the specified base notation
#+ will give an error message.

let "bad_oct = 081"
# numbers.sh: let: oct = 081: value too great for base (error token is "081")
#          Octal numbers use only digits in the range of 0 - 7.

exit 0
# Thanks, Rich Bartell and Stephane Chazelas, for clarification.
