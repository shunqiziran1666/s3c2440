# pinctrl for s3c2440
/home/juo/rd_s3c2440/s3c2440/kernel4.19/linux-4.19-rc3/Documentation/devicetree/bindings/interrupt-controller //三星中断控制器dts描述说明
/home/juo/rd_s3c2440/s3c2440/kernel4.19/linux-4.19-rc3/drivers/pinctrl/samsung //三星pinctrl源码
include/of.h  //设备树操作函数

1）devm_kmalloc()和devm_kzalloc()是具有资源管理的kmalloc()和kzalloc()。与所属设备关联，在probe函数中使用。当设备驱动被卸载时，该内存被自动释放，也可使用devm_kfree()函数直接释放。

2）kmalloc()和kzalloc()申请的内存必须调用kfree()释放。

