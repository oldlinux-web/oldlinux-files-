#!/bin/bash
# logon.sh: A quick 'n dirty script to check whether you are on-line yet.


TRUE=1
LOGFILE=/var/log/messages
# Note that $LOGFILE must be readable (chmod 644 /var/log/messages).
TEMPFILE=temp.$$
# Create a "unique" temp file name, using process id of the script.
KEYWORD=address
# At logon, the line "remote IP address xxx.xxx.xxx.xxx"
#                     appended to /var/log/messages.
ONLINE=22
USER_INTERRUPT=13

trap 'rm -f $TEMPFILE; exit $USER_INTERRUPT' TERM INT
# Cleans up the temp file if script interrupted by control-c.

echo

while [ $TRUE ]  #Endless loop.
do
  tail -1 $LOGFILE> $TEMPFILE
  # Saves last line of system log file as temp file.
  search=`grep $KEYWORD $TEMPFILE`
  # Checks for presence of the "IP address" phrase,
  # indicating a successful logon.

  if [ ! -z "$search" ] # Quotes necessary because of possible spaces.
  then
     echo "On-line"
     rm -f $TEMPFILE    # Clean up temp file.
     exit $ONLINE
  else
     echo -n "."        # -n option to echo suppresses newline,
                        # so you get continuous rows of dots.
  fi

  sleep 1  
done  


# Note: if you change the KEYWORD variable to "Exit",
# this script can be used while on-line to check for an unexpected logoff.

# Exercise: Change the script, as per the above note,
#           and prettify it.

exit 0


# Nick Drage suggests an alternate method:

while true
  do ifconfig ppp0 | grep UP 1> /dev/null && echo "connected" && exit 0
  echo -n "."   # Prints dots (.....) until connected.
  sleep 2
done

# Problem: Hitting Control-C to terminate this process may be insufficient.
#          (Dots may keep on echoing.)
# Exercise: Fix this.



# Stephane Chazelas has yet another alternative:

CHECK_INTERVAL=1

while ! tail -1 "$LOGFILE" | grep -q "$KEYWORD"
do echo -n .
   sleep $CHECK_INTERVAL
done
echo "On-line"

# Exercise: Consider the strengths and weaknesses
#           of each of these approaches.
