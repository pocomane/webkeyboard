#!/bin/sh

set -x

die(){
  echo "ERROR"
  exit -1
}

export CFLAGS=" -std=gnu99 -O2 "
export LDFLAGS=" -static -ldl "

if [ "$TARGET" = "" ]; then
  export TARGET="x86_linux"
fi

if   [ "$TARGET" = "x86_linux" ]; then
  echo "building for x86_linux"

  export DOWNLOAD_GCC_TOOLCHAIN="http://musl.cc/i686-linux-musl-native.tgz"
  export CC="$PWD/build/i686-linux-musl-native/bin/i686-linux-musl-gcc"
  export STRIP="$PWD/build/i686-linux-musl-native/bin/strip"

  # this should be the same of CC=" musl-gcc "
  # export CC=" cc -specs /usr/share/dpkg/no-pie-compile.specs -specs /usr/share/dpkg/no-pie-link.specs -specs /usr/lib/x86_64-linux-musl/musl-gcc.specs "

elif [ "$TARGET" = "arm_linux" ]; then
  echo "building for arm_linux"

  export DOWNLOAD_GCC_TOOLCHAIN="http://musl.cc/arm-linux-musleabihf-cross.tgz"
  export CC="$PWD/build/arm-linux-musleabihf-cross/bin/arm-linux-musleabihf-gcc"
  export STRIP="$PWD/build/arm-linux-musleabihf-cross/bin/arm-linux-musleabihf-strip"

else
  echo "unknown target '$TARGET'"
  exit -1
fi

mkdir -p build ||die
cd build ||die

echo "DOWNLOADING PACKAGES"
if [ "$DOWNLOAD_GCC_TOOLCHAIN" != "" -a "$(ls ".downloaded_$TARGET")" = "" ] ; then
  echo "downloading $DOWNLOAD_GCC_TOOLCHAIN"
  curl -L "$DOWNLOAD_GCC_TOOLCHAIN" --output cc_toolchain.tar.gz ||diw
  tar -xzf cc_toolchain.tar.gz ||die
  touch ".downloaded_$TARGET"
fi

echo "COMPILING"

$CC -static $CFLAGS -o webkeyboard.exe ../webkeyboard.c  $LDFLAGS ||die
$STRIP webkeyboard.exe ||die

echo "PACKING"

cp ../Readme.md ./wip ||die
export GIT_TAG_VERSION="$(cd ..; git rev-parse HEAD 2> /dev/null)"
if [ "$GIT_TAG_VERSION" = "" ]; then
  export GIT_TAG_VERSION="unknown"
fi 
echo -n "\n" >> ./wip ||die
echo -n "\nVersion report" >> ./wip ||die
echo -n "\n###############" >> ./wip ||die
echo -n "\n" >> ./wip ||die
echo -n "\nBuild timestamp $(date --rfc-3339=seconds)" >> ./wip ||die
echo -n "\nRepository revision $GIT_TAG_VERSION" >> ./wip ||die
echo -n "\nToolchain version $DOWNLOAD_GCC_TOOLCHAIN" >> ./wip ||die
echo >> ./wip ||die
cp ./wip ./Readme.md ||die

echo "Generating action script for MiSTer_misc distribution"
gen_script(){
  echo "#!/usr/bin/env bash" > "$2"
  echo "" >> "$2"
  echo "$1" >> "$2"
  echo '' >> "$2"
  chmod ugo+x "$2"
}
mkdir -o hook/action
gen_script "./webkeyboard.sh start" "hook/action/webkey_start.sh" ||die
gen_script "./webkeyboard.sh stop" "hook/action/webkey_stop.sh" ||die

mkdir -p deploy
tar -zcf "deploy/webkeyboard_$TARGET.tar.gz" Readme.md webkeyboard.exe hook/ ||die
if [ "$DEPLOY_ARCH_INDEPENDENT_FILES" = "yes" ]; then
  cp ../util/webkey_update.sh deploy/ ||die
  cp ../util/webkeyboard.sh deploy/ ||die
fi
ls -lha deploy

