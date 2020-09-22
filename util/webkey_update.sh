#!/usr/bin/env bash

# You can simply type the following line into the shell instead of running this file
curl -L -k -s https://raw.githubusercontent.com/pocomane/webkeyboard/master/util/webkey_mister.sh | bash -s update

# ---------------------------------------------------------------------------------

X=("${PIPESTATUS[@]}")
EXIT_CODE=${X[0]}
if [ "$EXIT_CODE" = "0" ]; then
  EXIT_CODE=${X[1]}
fi
if [ "$EXIT_CODE" != "0" ]; then
  echo "Error downloading the package ($EXIT_CODE)"
fi

read -n 1 -s -r -p "Press any key to continue"
exit $EXIT_CODE

