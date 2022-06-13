KERN_DIR = /home/juo/rd_s3c2440/s3c2440/kernel4.19/linux-4.19-rc3
all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= buttons.o
