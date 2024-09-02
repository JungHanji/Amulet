#!/bin/env sh
archivename='Amulet Release'
 
rm -rf "$archivename"
mkdir "$archivename"
 
cd "$archivename"
mkdir assets logs old output source src tools
mkdir src/include

tar -xf ../"$archivename.tar"
tar -xf 'Amulet Assets.tar'
tar -xf 'Amulet Source.tar'
tar -xf 'Amulet Tools.tar'
tar -xf 'Amulet.tar'
cd src/include
tar -xf ../../'Amulet Library.tar'
rm -f ../../*.tar