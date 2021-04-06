#!/usr/bin/env bash

# TODO : REMOVE THIS FILE : the update can be handled by https://github.com/pocomane/MiSTer_misc

# Test internet
ping -c 1 www.google.com > /dev/null
if [ "$?" != "0" ]; then
  echo "Network not found: check your internet connection or try later"
  exit 126
fi

# vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
# You can simply run the following command instead of running this file
#
curl -L -k https://raw.githubusercontent.com/pocomane/webkeyboard/master/util/webkeyboard.sh | bash -s update
#
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

X=("${PIPESTATUS[@]}")
EXIT_CODE=${X[0]}
if [ "$EXIT_CODE" = "0" ]; then
  EXIT_CODE=${X[1]}
fi
if [ "$EXIT_CODE" != "0" ]; then
  echo "Error downloading the package ($EXIT_CODE)"
fi

read -n 1 -s -r -p "Press any key to continue"
echo ""
exit $EXIT_CODE

