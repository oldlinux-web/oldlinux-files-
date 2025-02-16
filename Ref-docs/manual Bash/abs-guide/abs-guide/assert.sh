#!/bin/bash
# assert.sh

assert ()                 #  If condition false,
{                         #+ exit from script with error message.
  E_PARAM_ERR=98
  E_ASSERT_FAILED=99


  if [ -z "$2" ]          # Not enough parameters passed.
  then
    return $E_PARAM_ERR   # No damage done.
  fi

  lineno=$2

  if [ ! $1 ] 
  then
    echo "Assertion failed:  \"$1\""
    echo "File $0, line $lineno"
    exit $E_ASSERT_FAILED
  # else
  #   return
  #   and continue executing script.
  fi  
}    


a=5
b=4
condition="$a -lt $b"     # Error message and exit from script.
                          #  Try setting "condition" to something else,
                          #+ and see what happens.

assert "$condition" $LINENO
# The remainder of the script executes only if the "assert" does not fail.


# Some commands.
# ...
# Some more commands.

exit 0
