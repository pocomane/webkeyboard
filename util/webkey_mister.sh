#/usr/bin/env bash

GITHUB_REPO="pocomane/webkeyboard"
PACK_NAME_PATTERN="webkeyboard_arm.*tar.gz"
TREE_PATH="/media/fat"
EXE_NAME="webkeyboard.exe"
WORKING_SUB="misc/webkeyboard"
SCRIPT_SUB="Scripts"

# ---------------------------------------------------------------------------------

WORKING_DIR="$TREE_PATH/$WORKING_SUB"
SCRIPT_DIR="$TREE_PATH/$SCRIPT_SUB"
EXE_PATH="$WORKING_DIR/$EXE_NAME"
REPO_URL="https://github.com/$GITHUB_REPO"
API_URL="https://api.github.com/repos/$GITHUB_REPO"
PID=""

CURL=" curl -L -k -s "
TAR=" tar --no-same-owner --no-same-permissions "

die(){
  echo "ERROR $1"
  exit 127
}
  
mkdir -p "$WORKING_DIR" ||die "can not create the working directory '$WORKING_DIR'"
cd "$WORKING_DIR" ||die "can not enter in the working direrctory '$WORKING_DIR'"

wk_generate_script(){
  echo "#!/usr/bin/env bash" > "$2"
  echo "" >> "$2"
  echo "$1" >> "$2"
  echo '' >> "$2"
  echo '# ----------------------------------------------------------------' >> "$2"
  echo 'export EXIT_CODE="$?"' >> "$2"
  echo 'read -n 1 -s -r -p "Press any key to continue"' >> "$2"
  echo 'exit $EXIT_CODE' >> "$2"
  chmod ugo+x "$2"
}

wk_update(){
  echo "Installing the service"

  $CURL https://raw.githubusercontent.com/pocomane/webkeyboard/master/util/webkey_mister.sh > "$WORKING_DIR/webkey_mister.sh" ||die "can not access the working direrctory '$WORKING_DIR'"
  chmod ugo+x "$WORKING_DIR/webkey_mister.sh" ||die "can not access the working direrctory '$WORKING_DIR'"
  mkdir -p "$SCRIPT_DIR" ||die "can not create the script directory '$SCRIPT_DIR'"
  wk_generate_script "$WORKING_DIR/webkey_mister.sh start" "$SCRIPT_DIR/webkey_start.sh"
  wk_generate_script "$WORKING_DIR/webkey_mister.sh stop" "$SCRIPT_DIR/webkey_stop.sh"

  PACK_LIST=$($CURL -L -s $API_URL/releases/latest | sed -ne 's|^[ "]*browser_download_url[ "]*:[ "]*\([^"]*\)[ ",\t]*$|\1|p')
  PACK_URL=$(echo "$PACK_LIST" | grep "$PACK_NAME_PATTERN" | head -n 1)
  INFO="repo '$REPO_URL' / file '$PACK_URL'"
  $CURL "$PACK_URL" > "./tmp.tar.gz" ||die "can not download $INFO"
  $TAR -xzf tmp.tar.gz ||die "can not unpack $INFO"
  chmod ugo+x "$EXE_PATH" ||die "can not make '$EXE_PATH' executable"
  rm -f tmp.tar.gz
  if [ "$(ls $EXE_PATH)" = "" ]; then
    false ||die "invalid package $INFO" 
  fi

  if [ "$?" = "0" ]; then
    echo "Installation completed"
  else
    false ||die "Installation failed"
  fi
}

wk_find_pid(){
  PID=$(ps ax -o pid,comm | grep "$EXE_NAME"| grep -oh '^[ \t]*[0-9]*' | grep -oh '[0-9]*')
#  echo "instance list [$PID]"
}

wk_start(){
  wk_find_pid
  if [ "$PID" != "" ]; then
    false ||die "there is an instance already running"
  fi
  ( (nohup $EXE_PATH 8081 > ./webkeyboard.log 2>&1) &) ||die "can not start the instance"
  sleep 1
  tail ./webkeyboard.log
}

wk_stop(){
  wk_find_pid
  if [ "$PID" = "" ]; then
    false ||die "can not find any running instance"
  fi
  kill -09 "$PID" ||die "can not stop process '$PID'"
}

wk_info(){
  echo "Usage Summary."
  echo "To download and update the service:"
  echo "  $0 update"
  echo "To start the service:"
  echo "  $0 start"
  echo "To stop the service:"
  echo "  $0 stop"
}

# main dispatch
if [ "$#" = "0" ]; then
  wk_info
else
  case $1 in
    "update")
       wk_update;;
    "start")
       wk_start;;
    "stop")
       wk_stop;;
    *)
       echo "Invalid option"
       wk_info
       false ||die "invalid option"
       ;;
  esac
fi

