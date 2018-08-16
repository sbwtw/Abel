<!-- TOC -->

- [1. 说明](#1-说明)
- [2. 功能](#2-功能)
- [3. 核心组件](#3-核心组件)
- [4. 外部接口](#4-外部接口)
- [5. Block Diagram](#5-block-diagram)
- [6. Power](#6-power)
  - [6.1. 系统输入](#61-系统输入)
  - [6.2. USB 2.0 Host输出](#62-usb-20-host输出)
  - [6.3. BLE电源](#63-ble电源)
  - [6.4. SATA](#64-sata)
- [7. Button](#7-button)
- [8. LED](#8-led)

<!-- /TOC -->

# 1. 说明

本文档为Abel的硬件原理性定义，不包含产品的外观和结构特性。

Abel的硬件原理会对应两款不同的硬件产品。


两者仅在结构上有区别，硬件原理全同，每种结构可能对应多个SKU，区别在于内存配置，是否预置硬盘，和硬盘的种类和容量差异。

# 2. 功能

Abel是闻上科技自主开发的NAS/家用私有云产品。

**硬件**

1. 预置或用户处安装的硬盘；
2. WiFi, 802.11ac, AC1200，2T2R，外置双天线（whip）；
3. Bluetooth Low Energy (BLE) 5.0；
4. USB 2.0 Host，1路；
5. 内置Crypto (ECC) IC，用于IoT设备认证；
6. 适配器外置，12V 3A电源输入，筒形（Barrel）连接器；
7. 电源开关，蓝牙按键；
8. 电源指示灯，蓝牙指示灯，WiFi指示灯；

**软件与服务**

1. 云端帐号与设备管理、手机App、PC/MAC上的客户端程序；
2. 可以自动备份手机内容；
3. 可以自动备份电脑中指定内容；
4. 可以方便的在手机和PC/MAC之间传输内容；
5. 可以在各种客户端上浏览NAS中的全部内容；
6. 可以远程访问；
7. 支持多账户；
8. 支持Samba共享和DNLA（限指定存储位置）；
9. 支持将NAS中的内容备份到外置存储设备上；
10. 支持访问外置存储设备，在NAS和外置存储设备间传输文件；

# 3. 核心组件

1. CPU/PMU: RK3328 + RK805；
2. RAM: LPDDR3 or DDR4，1GB/2GB可选；
3. EMMC: 16GB；
4. WIFI: fn-link（欧智通） 6222D-UUB模块，选ipex接口版本；rtl8822bu方案；
5. BLE: 基于TI cc2640r2f方案，使用模块，模块供应商和型号待定；
6. Crypto (ECC) IC: Microchip/Atmel atecc508a；

# 4. 外部接口

1. 电源接口
2. USB Host 2.0 x 1，该接口按照rockchip的产品设计为OTG接口，可用Host-To-Host Cable烧录emmc
3. WiFi（ipex on wifi module）

# 5. Block Diagram

```
---------      ---------
|       |      |       |  power on
|  CPU  |------|  PMU  |------------
|       |      |       |           |
|       |      ---------           |
|       |                         / \  OR
|       |      ---------         /   \
|       | USB  |       |         |   |
|       |------|  WIFI |         |   ------------- Power Button
|       |      |       |         |
|       |      ---------         |
|       |      ---------         |
|       | UART |       |---------- GPIO (Open Drain)
|       |------|  BLE  |
|       |      |       |-------------------------- Bluetooth Button & LED
|       |      ---------
|       |      ---------
|       | I2C  |       |
|       |------|  ECC  |
|       |      |       |
|       |      ---------
|       |      ---------
|       | USB3 |       |
|       |------|  SATA |----> 接SATA硬盘
|       |      |       |
|       |      ---------
---------
```

# 6. Power

## 6.1. 系统输入

系统输入电源要求12V 3A规格

## 6.2. USB 2.0 Host输出

USB 2.0 Host输出要求最大1.2A限流

## 6.3. BLE电源

BLE模块的电源要求绕过PMU，使用独立的电源路径供电，在系统12V电源上电时即有供电。

BLE模块可以通过GPIO独立控制PMU的Power On，该信号和电源按键是OR逻辑。

## 6.4. SATA

SATA使用22pin（15+7）的标准Combo接口；其中12V供电由Power Adapter直供，但应该有NMOS/PMOS开关，即PMU向系统输出时供电。

# 7. Button

1. Power Button常规接PMU（需要check参考板）；
2. Bluetooth Button由BLE模块控制；

# 8. LED

待定义













