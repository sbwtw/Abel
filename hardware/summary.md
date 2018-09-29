# BSP及BLE工作汇总

20180929

## BSP
编译u-boot, linux kernel，在rock64板子上启动以Ubuntu-base为rootfs的linux系统
+ 编译U-boot, arm-trusted-firmware
+ 编译kernel，基于4.4版本内核，主要是添加了额外的WiFi驱动RTL8822BU
+ 制作rootfs，基于ubuntu-base-16.04.5-base-arm64，用chroot的方式安装必要的package
+ 分区与启动
  1. 基于emmc启动，emmc上设计P, A, B三个分区，uboot从P分区读取启动参数，进而启动A或B分区的系统，额外的SSD盘作为数据盘
  2. 基于SPI Flash启动，uboot在SPI上，额外的SSD盘上分四个分区，即P, A, B及数据分区
+ 制作SD卡

详细每个步骤的过程见文档 [BSP_Note](https://github.com/wisnuc/Abel/blob/master/hardware/BSP/BSP_Note.md)
制作rootfs的脚本为 [build-abel-rootfs-emmc-base.sh](https://github.com/wisnuc/Abel/blob/master/hardware/BSP/build-abel-rootfs-emmc-base.sh)
制作sd卡的脚本为 [prepare_sd.sh](https://github.com/wisnuc/Abel/blob/master/hardware/BSP/prepare_sd.sh)

## BLE
选用TI CC2640R2F蓝牙模块，修改和编译BLE固件
+ 基于TI提供的例子spp_ble_server，添加额外的Profile及处理uart通讯的逻辑，主要添加和修改的C代码见[C](https://github.com/wisnuc/Abel/tree/master/hardware/BLE/C)
+ 开启ROM boot loader 并暂时设置PIN Number 为13
+ 使用CCS编译源代码，可直接刷入BLE内，也可以生成.bin格式的镜像文件后，手动刷入设备
+ 刷写BLE固件的基本代码已用JS实现，见[flash.js](https://github.com/wisnuc/Abel/blob/master/hardware/BLE/flash.js)
+ bled.js、BLE、客户端的通讯协议设计见[protocol.md](https://github.com/wisnuc/Abel/blob/master/hardware/BLE/protocol.md)
  1. bled.js与客户端之间以JSON格式的数据包通讯，以`\n`做分割符
  2. bled.js与BLE间存在周期为1s的心跳连接，并记有session，客户端可以通过GATT获得bled.js与BLE的连接状态
  3. BLE与客户端连接或断开时，bled.js都会获得相应的消息
+ bled.js代码已基本完成，但是还需要加入具体的业务逻辑及GPIO相关的逻辑，代码见[bled.js](https://github.com/wisnuc/Abel/blob/master/hardware/BLE/bled.js)

## net 模块
控制设备wifi的模块，代码见[net.js](https://github.com/wisnuc/Abel/blob/master/hardware/BLE/net.js)
+ 实现控制wifi的scan、connect、disconnect操作及wifi连接状态的监听
+ 结合bled.js，实现通过手机蓝牙获取wifi扫描结果，发送连接wifi命令的功能，代码见[BLE_wifi_Test.js](https://github.com/wisnuc/Abel/tree/master/hardware/BLE/BLE_wifi_Test.js)
