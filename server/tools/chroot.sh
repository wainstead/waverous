# on a Fedora 14 system, here's how I created a chrooted environment
# to run the server in...

# path to your project, which should be compiled
MOOBIN=~swain/public_html/projects/waverous/server

# path to the chroot directory you will use
CHROOTDIR=/waverous/chroot

echo "To remove the chrooted environment:"
echo "rm -rf $CHROOTDIR"

mkdir -p $CHROOTDIR
cd $CHROOTDIR
mkdir -p lib usr/lib
mkdir bin sbin logs databases

cp $MOOBIN/moo $CHROOTDIR/sbin/
cp $MOOBIN/databases/* $CHROOTDIR/databases/

# for the moo binary: run 'ldd moo' to see the list of libraries it
# needs, then copy them to $CHROOTDIR...
cp /lib/ld-linux.so.2      lib/
cp /lib/libc.so.6          lib/
cp /lib/libcrypt.so.1      lib/
cp /lib/libgcc_s.so.1      lib/
cp /lib/libm.so.6          lib/
cp /usr/lib/libexpat.so.0  usr/lib/
cp /usr/lib/libstdc++.so.6 usr/lib/


# for bash, if you want to run restart.sh...
# cp /lib/libtinfo.so.5 lib/
# cp /bin/bash bin/
# cd bin
# chmod a+x bash
# ln -s bash sh
# cd ..

# running the server; but running it via restart.sh would be better,
# because your changes are lost otherwise
#chroot $CHROOTDIR /sbin/moo -l /logs/my.log /databases/LambdaCore-12Apr99.db /databases/LambdaCore-12Apr99.mine.db
# on slim:
# /usr/sbin/chroot /waverous/chroot /sbin/moo -l /logs/my.log /databases/LambdaCore-12Apr99.db /databases/LambdaCore-12Apr99.mine.db