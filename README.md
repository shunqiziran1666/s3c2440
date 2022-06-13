# JZ2440开发环境
安装交叉工具链、编译uboot、编译kernel、设置NFS下载。
用到的文件：uboot.bin及对应patch文件、对应版本kernel源码及patch文件、交叉工具链。

## 编译
1. 安装交叉编译工具链，安装完交叉工具后，进入该目录下，进入bin目录下，然后pwd拿到bin目录路径。

2. 设置环境变量（该方法source完之后便永久对当前用户有效）
```c
1. vim ~/.bashrc  //最后一行添加以下语句
2. export PATH=$PATH:/home/book/${工具链路径}/bin  //path后的路径便是你的bin目录路径。  
3. source ~/.bashrc  //保存退出，该命令执行完毕后make便会自动去此目录下拿工具链进行编译。  
4. echo $PATH  //查看当前环境变量，环境变量决定shell程序到哪些路径寻找
``` 

3. 然后就可以执行 arm-linux-gnueabi-gcc -v 观看到版本号了

使用韦东山的4.9的工具链是用来编译内核和uboot的，如果编译APP和文件系统用4.3的工具链。

4. 编译APP 
```c
   1. export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/usr/local/arm/4.3.2/bin
   2. export PATH=/usr/local/arm/4.3.2/bin:$PATH

注：以上两条命令是等效的，但是这种方法只是临时生效，只会对当前终端有效，所以还是改bashrc比较好

然后就可以执行 arm-linux-gcc -v 观看到版本号了
```

另外需要先编译uboot，然后将编译uboot生成的mkimage文件（tools下面）拷贝到/bin目录下。sudo cp mkimage /bin。  
可能会提示mkimage没有权限，这时我们可以sudo chmod 777 mkimage


## 如何挂接网络文件系统---NFS
1. 需要配置服务器允许挂载，在哪里加上下面这句话？在/etc/exports里加上，注意 / 表示根目录！！！
```c
/home/juo/rd_s3c2440/fs/fs_mini_mdev_new *(rw,sync,no_root_squash)
//注意，如果根文件系统路径变了，需要重新修改
```

2. 保存退出，输入如下命令重启nfs
```c
sudo /etc/init.d/nfs-kernel-server restart
```

3. 首先需要有一个正常的文件系统，能够进入linux

4. 在文件系统的/etc/init.d rcS文件中加入如下命令：  
```c
ifconfig eth0 up  
ifconfig eth0 192.168.50.188
```

所以，uboot的ip地址与linux的IP地址不一样，nfs挂载设置的是linux的IP地址！！！

//设置ip地址
```c
set bootargs noinitrd root=/dev/nfs nfsroot=192.168.50.62:/home/juo/rd_s3c2440/s3c2440/fs/fs_mini_mdev_new ip=192.168.50.188:192.168.50.62:192.168.50.201:255.255.255.0::eth0:off init=/linuxrc console=ttySAC0,115200
```

//保存命令
```c
saveenv
```

5. 关闭防火墙，sudo ufw disable

6. 如果没有uboot，使用oflash工具先烧写uboot，选择boot the system 即uboot选项b，不能从uboot中输入boot启动！

如果不能挂载，先在本地ubuntu试试能不能自己挂载自己:  
sudo mount -t nfs 192.168.50.62:/home/juo/rd_s3c2440/s3c2440/fs/fs_mini_mdev_new /mnt  
若还不行，烧写dtb fs kernel到开发板，然后开发板内手动试试能不能挂载:
```c
mount -t nfs -o nolock 192.168.50.62:/home/juo/rd_s3c2440/s3c2440/fs/fs_mini_mdev_new/fs_mini_mdev_new /mnt
```






-------------------------------------------------------------------------------------------

## uboot常用命令
1. setenv 设置环境变量，环境变量有哪些可通过printenv打印出来
2. 设置完之后需要saveenv保存
3. 例子：setenv ipaddr 192.168.50.188

```C
bootargs=noinitrd root=/dev/mtdblock4 rw init=/linuxrc console=ttySAC0,115200（默认）
```


4. 默认uboot环境变量:
```c
bootargs=noinitrd root=/dev/mtdblock4 rw init=/linuxrc console=ttySAC0,115200
bootcmd=nand read.jffs2 0x30007FC0 kernel; nand read.jffs2 32000000 device_tree; bootm 0x30007FC0 - 0x32000000
bootdelay=2
baudrate=115200
ethaddr=08:00:3e:26:0a:5b
ipaddr=192.168.1.17
serverip=192.168.1.11
netmask=255.255.255.0
stdin=serial
stdout=serial
stderr=serial
mtdids=nand0=nandflash0
mtdparts=mtdparts=nandflash0:256k@0(bootloader),128k(device_tree),128k(params),4m(kernel),-(root)
partition=nand0,0
mtddevnum=0
mtddevname=bootloader
```

5. 恢复uboot默认环境变量
```c  
set bootargs noinitrd root=/dev/mtdblock4 rw init=/linuxrc console=ttySAC0,115200
```

6. 打印环境变量  
printenv

## nand erase params

## 烧写完kernel、uboot然后nfs挂载根文件系统后一直提示bad magic number原因
是因为没有烧写设备树文件！！！

## nfs烧写设备树与uimage
```c
nfs 30000000 192.168.50.62:/home/juo/rd_s3c2440/s3c2440/fs/fs_mini_mdev_new/uImage; nfs 32000000 192.168.50.62:/home/juo/rd_s3c2440/s3c2440/fs/fs_mini_mdev_new/jz2440_irq.dtb; bootm 30000000 - 32000000
```

## nfs下载
nfs下载仅对本次生效，因为是烧写到SDRAM的

## nfs挂载文件不同步的问题
也怪自己看视频不仔细，也怪韦老师坑了，韦老师在设备树文件里设置了chosen节点，该节点里设置了bootargs参数，这个参数覆盖了我们在uboot里的参数修改，最终导致一直不是在挂载的文件系统里运行的，所以改了设备树的参数后，恢复正常！！！

## 打开内核调试信息
```c
echo "7 4 1 7 " > /proc/sys/kernel/printk  
demsg -n 8 //dmesg命令和上面的原理上是一样的
```

## sys/kernel/debug文件空的情况
1. 查看内核是否打开 debugfs 的支持，make menuconfig 然后搜索 "Debug Filesystem"，是[*]则为已打开
2. 挂载 debugfs （在开发板的shell中输入命令）:
```c
mount -t debugfs debugfs /sys/kernel/debug
```

3. 查看挂载 debugfs 是否成功:
```c
mount | grep debug 
出现：debugfs on /sys/kernel/debug type debugfs (rw,relatime) 即为成功
```

## rmmod无法卸载问题
需要在lib目录下创建相应的文件夹，但是提示mkdir: can't create directory '/lib/modules/': Read-only file system

//该命令可以解决readonly问题
```c  
mount -o remount,rw /
```

//该命令创建相关文件夹
```c
mkdir -p /lib/modules/$(uname -r)
```	


做完以上操作若rmmod不报错，但是还是驱动还是没有卸载，则把驱动的.ko去掉。

## boot与uboot中b选项不一致问题
uboot命令中boot不是nfs挂载的文件系统、且自行启动也不是进入nfs挂载的目录
b则是进入挂载的文件系统