# '-H FORMAT' archive format, 
#  newc: SVR4 portable format

cd rootfs
find . | cpio -o -H newc > ../initramfs.cpio
cd ..