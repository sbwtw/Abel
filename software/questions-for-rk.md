上海闻上科技有限公司
马天夫 (lewis.ma@winsuntech.cn) 15618429080

项目代号: Abel

# 简介

Abel是一款家用NAS产品；产品主要通过http服务向客户端程序提供文件访问和管理服务，同时集成云服务，为用户提供远程访问和设备管理能力，并具有全局统一的用户帐号体系。产品为无头设计，无视频音频接口。

Abel的硬件基于瑞芯微的RK3328平台，主要参考arm/linux社区的开源硬件板设计，包括：

1. Pine64的Rock64
2. t-firefly/libre computer的Renegade ROC-RK3328-CC

两者硬件上大同小异。

Abel内置16G emmc，操作系统和应用程序位于emmc上。RK3328具有一路USB 3 Host，该接口会通过usb-sata bridge芯片接硬盘或SSD。

Abel的软件为嵌入式linux，主要参考RK官方的u-boot/kernel和ROCK64的armbian版本（基于ubuntu base 16.04的flavor），内核版本4.4，与RK官方的开源Linux内核版本一致，也可能考虑使用mainline内核，考虑使用mainline内核的原因是Abel的NAS服务依赖于btrfs文件系统，btrfs文件系统有很多bug或者功能限制，越新的内核版本越好。

# 问题

## 内存

我们目前对内存配置的理解是，在uboot启动的最初会配置内存参数，该二进制为RK提供，社区很少修改，所以为规避项目风险，需要RK提供可支持的内存颗粒型号列表。

## 系统分区

http://opensource.rock-chips.com/wiki_Partitions

Abel拟修改原RK参考设计中的分区表；其中boot前面的分区均不变；boot分区不用；rootfs分区改为4个分区：

1. 第一个分区放置一个精简的rootfs，含kernel，功能相当于一个功能更强一点的boot loader，称kbl分区（kernel boot loader）；
2. 第二和第三个分区是实现系统功能的rootfs，均含kernel，分别称为a/b分区，用于实现a/b模式的系统升级；
4. 第四个分区称p分区，用于存放持久化数据（persistent data），包括a/b分区的选择启动标记；

需要做的修改：

1. 启动时uboot从kbl分区内载入内核、ramdisk（initramfs）和启动参数，使用ext4load；使用kbl作为rootfs启动；
2. kbl分区的init或基础服务会检查p分区的配置，据此启动a或b分区，使用kexec；

问题：

1. 删除boot分区是否有问题？
2. uboot里需要修改源代码吗？还是uboot可以直接probe出修改的gpt分区表，用script控制即可？

**对量产工具的影响**

量产应该是使用usb host-to-host线从usb otg口下载镜像至emmc；如果按照上述设计修改了分区表：

1. 是否需要修改rkdeveloptool源码才能支持新的partition格式，还是只要修改partition设置的txt文件即可？
2. 修改分区表是否对量产有其他潜在的影响？

## 关于串口通讯

1. 系统外部存在一个蓝牙低功耗芯片（BLE）与CPU串口通讯。请问CPU是否会深度休眠需要GPIO唤醒？还是不需要？我们期望该串口可以直接使用，对外部芯片而言不需要先通过GPIO唤醒MPU然后才能通讯？
2. 高速通讯需要串口的CTS/RTS信号支持，请问内核驱动是否支持？
3. 请问哪个串口是目前被官方代码固定使用作为uboot和kernel的console的？我们选择其他串口实现设备外部通讯，避免冲突。

## 其他

按照上述设计，是否还有什么其他隐患？

谢谢。





