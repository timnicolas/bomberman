#!/bin/sh

APPNAME=bomberman.app
DMG=$1/bomberman.dmg

DIR=/tmp/__tmpBomberman
TMP_DMG=/tmp/__tmpBomberman.dmg
CONT="$DIR/$APPNAME/Contents/"

echo "create $DIR/$APPNAME"

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

echo "create $DMG"
rm -rf $DMG
hdiutil create $TMP_DMG -ov -volname "bomberman" -fs HFS+ -srcfolder $DIR
hdiutil convert -format UDZO -o $DMG $TMP_DMG

rm -rf $DIR
rm -rf $TMP_DMG
echo "created"

exit 0
