obj-m := leddrv.o
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
all:
	make -C /home/embdtut/work/kernel/linux_kernel SUBDIRS=/nfs_root/leddev modules
