#烧写步骤
	1.使用EOP烧写u-boot.bin到JZ2440的nor flash或nand flash
	2.启动u-boot, 在串口工具中输入相应菜单命令, 使用dnw_100ask.exe发送对应文件

				菜单                            要发送的文件
		[k] Download Linux kernel uImage          uImage
		[t] Download device tree file(.dtb)       jz2440.dtb
		[y] Download root_yaffs image             fs_mini_mdev_new.yaffs2

	3. 烧写完毕即可重启进入板上LINUX系统。
	
	
	
1. 编译器的选择:
一个完整的Linux系统包含三部分: u-boot, kernel, root filesystem.
a. 对于u-boot:
我们仍然使用u-boot 1.1.6, 在这个版本上我们实现了很多功能: usb下载,菜单操作,网卡永远使能等, 不忍丢弃.

b. 对于kernel:
我下载了目前(2018.09.19)最新的内核 (4.19)

c. 对于root filesystem
中文名为"根文件系统", 它包含一些必须的APP, 一些动态库。
一般来说这些动态库是从工具链里的lib目录复制得到的,
当然也可以自己去编译glibc等库。

在编译u-boot和kernel时, 我们可以使用新的工具链, 
只要这个工具链支持ARM9的指令集(armv4)就可以(这通常可以通过编译参数来指定使用特定的指令集).
工具链可以从某些网站上下载，并不需要自己去制作。
比如可以访问这个网站: https://releases.linaro.org/components/toolchain/binaries/4.9-2017.01/arm-linux-gnueabi/
下载: gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi.tar.xz

但是在制作根文件系统时, 实际上我们是编译各种APP, 
这些APP要用到一些动态库, 为了方便一般直接使用工具链中lib目录里的库。
新版工具链的lib库一般是支持新的芯片，比如cortex A7,A8,A9，并不支持ARM9。
所以在制作根文件系统、编译APP时我们还得使用比较老的工具链: arm-linux-gcc-4.3.2.tar.bz2


2. 通过设置PATH环境变量来选择使用某个工具链:
2.1 安装工具链:
    这非常简单, 解压即可:
    sudo tar xjf arm-linux-gcc-4.3.2.tar.bz2 -C /                     (解压到根目录, /usr/local/arm/4.3.2/bin/下就是工具链)
    tar xJf gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi.tar.xz  (解压到当前目录, 假设/work/system/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi/bin下就是工具链)
    
    注意: "/work/system" 请自行替换为你的实际目录
    
2.2 设置环境变量使用某个工具链:
a. 要使用arm-linux-gcc 4.3.2, 执行如下命令:
    export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/usr/local/arm/4.3.2/bin
   然后就可以执行 arm-linux-gcc -v 观看到版本号了

b. 要使用arm-linux-gnueabi-gcc 4.9.4, 执行如下命令:
    export  PATH=PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/home/book/GNU/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi/bin
   然后就可以执行 arm-linux-gnueabi-gcc -v 观看到版本号了
   


2. u-boot的编译:
a. 首先设置环境变量使用要使用arm-linux-gnueabi-gcc 4.3.2
b. 
把源文件u-boot-1.1.6.tar.bz2、补丁文件u-boot-1.1.6_device_tree_for_jz2440_add_fdt_20181226.patch放在同一个目录,
执行如下命令:
tar xjf u-boot-1.1.6.tar.bz2                                   // 解压
cd u-boot-1.1.6                  
patch -p1 < ../u-boot-1.1.6_device_tree_for_jz2440_add_fdt_20181226.patch       // 打补丁
make 100ask24x0_config                                         // 配置
make                                                           // 编译, 可以得到u-boot.bin

3. kernel的编译:
a. 首先设置环境变量使用要使用arm-linux-gnueabi-gcc 4.3.2
b. 
把源文件linux-4.19-rc3.tar.gz、补丁文件linux-4.19-rc3_device_tree_for_jz2440.patch放在同一个目录,
执行如下命令:
tar xzf linux-4.19-rc3.tar.gz                                   // 解压
cd linux-4.19-rc3                  
patch -p1 < ../linux-4.19-rc3_device_tree_for_jz2440.patch      // 打补丁
cp config_ok .config                                            // 配置
make uImage                                                     // 编译, 可以得到arch/arm/boot/uImage
make dtbs                                                       // 编译, 可以得到arch/arm/boot/dts/jz2440.dtb

注意: 
a. 如果提示"mkimage not found", 先编译u-boot, 把tools/mkimage复制到/bin目录
b. 如果提示"openssl/bio.h: No such file or directory"
   先确保你的ubuntu可以上网, 然后执行如下命令:
   sudo apt-get update
   sudo apt-get install libssl-dev
   
4. 制作root filesystem : 
   可以直接使用映象文件: fs_mini_mdev_new.yaffs2   
   
   如果想自己制作，请参考视频: 
   从www.100ask.net下载页面打开百度网盘,
   打开如下目录:
        100ask分享的所有文件
            009_UBOOT移植_LINUX移植_驱动移植(免费)
                毕业班第3课_移植3.4.2内核
                    毕业班第3课第2节_移植3.4.2内核之修改分区及制作根文件系统.WMV



5. 烧写
a. 使用EOP烧写u-boot.bin到JZ2440的nor flash或nand flash
b. 启动u-boot, 在串口工具中输入相应菜单命令, 使用dnw_100ask.exe发送对应文件
   
       菜单                            要发送的文件
[k] Download Linux kernel uImage          uImage
[t] Download device tree file(.dtb)       jz2440.dtb
[y] Download root_yaffs image             fs_mini_mdev_new.yaffs2

   烧写完毕即可重启进入板上LINUX系统。
   




