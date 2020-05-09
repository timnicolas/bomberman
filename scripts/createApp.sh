#!/bin/sh

APPNAME=bomberman.app
DMG=$1/bomberman.dmg

DIR=/tmp/__tmpBomberman
TMP_DMG=/tmp/__tmpBomberman.dmg
CONT="$DIR/$APPNAME/Contents/"

echo "\x1B[4mCREATE $DIR/$APPNAME\x1B[0m"

mkdir $DIR/
mkdir $DIR/$APPNAME
mkdir $CONT
mkdir $CONT/MacOS
echo '#!/bin/sh

BASEDIR=$(dirname "$0")
cd $BASEDIR/..
./bomberman > bomberman-log.txt 2>&1

exit 0' > $CONT/MacOS/bomberman
chmod 755 $CONT/MacOS/bomberman

cp bomberman $CONT
cp -r bomberman-assets $CONT
rm -rf $CONT/bomberman-assets/.git
cp -r shaders $CONT

fileicon set $DIR/$APPNAME bomberman-assets/imgs/icon-app.png
ln -s /Applications $DIR/Applications

echo "\x1B[4mIMPORT LIBS ON APP TO FRAMEWORK\x1B[0m"

mkdir -p $CONT/Frameworks

import_lib()
{
    array=("$@")
    echo "import $1:"
    printf "change references: "
    for i in "${array[@]}"
    do
        libpath=$(otool -L $1 | tail -n +2 | grep $i | tr -d "\t" | grep -o '^[^ ]*' | tr -d "\n")
        if [ ! -z "$libpath" ]
        then
            printf "."
            libname=$(basename $libpath)
            if [ ! -f "$CONT/Frameworks/$libname" ]
            then
                cp $libpath $CONT/Frameworks
                chmod +w $CONT/Frameworks/$libname
                install_name_tool -id @executable_path/Frameworks/$libname $CONT/Frameworks/$libname
                chmod -w $CONT/Frameworks/$libname
            fi
            chmod +w $1
            install_name_tool -change $libpath @executable_path/Frameworks/$libname $1
            chmod -w $1
        fi
    done
    echo ""
}

import_sub_lib()
{
    for filename in $CONT/Frameworks/*.dylib
    do
        arra=("$@")
        import_lib $filename ${arra[@]}
    done
}

declare -a arr1=("sdl2/lib/libSDL2" "sdl2_mixer/lib/libSDL2_mixer" "assimp/lib/libassimp" "freetype/lib/libfreetype" "boost/lib/libboost_filesystem")
import_lib "$CONT/bomberman" ${arr1[@]}

cp ~/.brew/lib/libIrrXML.dylib $CONT/Frameworks
declare -a arr2=("libIrrXML.dylib" "sdl2/lib/libSDL2" "libmodplug/lib/libmodplug" "libvorbis/lib/libvorbisfile" "libvorbis/lib/libvorbis" "libpng/lib/libpng16")
import_sub_lib ${arr2[@]}

declare -a arr3=("libogg/lib/libogg" "lib/libvorbis")
import_sub_lib ${arr3[@]}

echo "\x1B[4mCREATE $DMG\x1B[0m"
rm -rf $DMG
hdiutil create $TMP_DMG -ov -volname "bomberman" -fs HFS+ -srcfolder $DIR
hdiutil convert -format UDZO -o $DMG $TMP_DMG

rm -rf $DIR
rm -rf $TMP_DMG
echo "created"

exit 0
