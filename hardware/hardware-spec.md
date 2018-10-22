Project Abel

Copyright 上海闻上信息科技有限公司 2018

<!-- TOC -->

- [1. 版本](#1-版本)
- [2. 项目简介](#2-项目简介)
- [3. 系统概述](#3-系统概述)
    - [3.1. 硬件规格](#31-硬件规格)
    - [3.2. 软件规格](#32-软件规格)
- [4. 硬件](#4-硬件)
    - [4.1. 需要删除的外设或接口](#41-需要删除的外设或接口)
    - [4.2. 需要增加的外设或接口](#42-需要增加的外设或接口)
    - [4.3. MMC卡座](#43-mmc卡座)
    - [4.4. SPI NOR Flash](#44-spi-nor-flash)
    - [4.5. SATA](#45-sata)
    - [4.6. WiFi](#46-wifi)
    - [4.7. BLE](#47-ble)
        - [4.7.1. BLE供电](#471-ble供电)
        - [4.7.2. BLE需能控制PMU的Power开关](#472-ble需能控制pmu的power开关)
        - [4.7.3. BLE需能感知PMU的sys power输出](#473-ble需能感知pmu的sys-power输出)
        - [4.7.4. MPU与BLE芯片的通讯](#474-mpu与ble芯片的通讯)
        - [4.7.5. BLE控制LED灯](#475-ble控制led灯)
        - [4.7.6. MPU对BLE芯片的控制](#476-mpu对ble芯片的控制)
        - [4.7.7. BLE的JTAG接口](#477-ble的jtag接口)
        - [4.7.8. BLE小结](#478-ble小结)
    - [4.8. atecc安全芯片](#48-atecc安全芯片)
    - [4.9. MPU的console接口](#49-mpu的console接口)
    - [4.10. MPU信号汇总](#410-mpu信号汇总)
    - [4.11. 其他](#411-其他)

<!-- /TOC -->

# 1. 版本

20181015 闻上马天夫 lewis.ma@winsuntech.cn 微信 unwiredgrain

# 2. 项目简介

Abel项目是一款家用NAS设备，提供云盘、照片备份等功能；用户可以远程访问设备，也可以把设备资源的一部分分享给其他用户；

该设备与闻上云服务和云账户系统紧密集成。

# 3. 系统概述

## 3.1. 硬件规格

1. 基于RK3328平台，配套RK805电源管理芯片（PMU）
2. 2G memory (LPDDR3, ddr3L or ddr4)
3. 64Mbit SPI Nor flash 
4. WiFi 802.11ac (AC1200)
5. Bluetooth LE
6. USB 2.0 Type-C接口 x 1，固定为device模式
7. 1个电源开关（同时也是GPIO按键）
8. 1个3色LED
9. Type-C接口也是系统的供电接口

## 3.2. 软件规格

1. Linux内核4.4或mainline
2. Ubuntu base 18.04 / arm64
3. 自研NAS服务器软件

# 4. 硬件

硬件参考设计使用Pine64社区的ROCK64板：

+ 产品页：https://www.pine64.org/?page_id=7147
+ Pine64官方资源：https://www.pine64.org/?page_id=7175
+ Wiki：http://wiki.pine64.org/index.php/ROCK64_Main_Page
+ Rock64板载接口信号定义：http://files.pine64.org/doc/rock64/ROCK64_Pi-2%20_and_Pi_P5+_Bus.pdf

从上述资源中可以获得ROCK64的原理图。

## 4.1. 需要删除的外设或接口

以下外部设备从ROCK64设计中删除：
+ 以太网
+ 音频
+ HDMI
+ 红外
+ USB 3连接器
+ 原有两路USB 2接口Type A连接器删除，host一路板载接wifi，otg一路接Type C

## 4.2. 需要增加的外设或接口

+ USB 3通过USB-SATA bridge芯片接SATA接口；
+ USB 2 Host接WiFi模块；
+ USB 2 OTG接Type C接口，固定为Device模式；
+ I2C1（Pi-BUS的Pin 27/28）接atecc安全芯片；
+ UART1（Pi-BUS的Pin 15/16/18/22）接蓝牙BLE芯片；
+ UART2为调试接口，板上留2.54mm间距过孔焊盘，无连接器；

## 4.3. MMC卡座

MMC沿用ROCK64设计，量产产品贴片，需要一个push-push连接器，位置在版边，正反均可；

该卡座在系统内部，并不提供给消费用户使用，但在出货时贴片，主要用于维修和玩家刷机。

## 4.4. SPI NOR Flash

spi nor flash用于存储boot loader，包括uboot，用于系统启动，是系统在正常使用时的启动方式；

RK3328无可配置的boot pin修改启动顺序，芯片的boot rom固定了启动逻辑，依次为：

1. emmc
2. spi nor flash
3. spi nand flash
4. mmc
5. usb otg

设备在工厂烧录spi可以直接使用治具烧录、贴片前预烧录、或者首次启动插入mmc卡烧录spi等不同方式。具体方式待定，需临滴给出建议。

在使用或维修过程中，如果需要再次烧录spi，需要有一种办法在启动时屏蔽SPI信号，社区里常用的做法是用镊子短路其clock，这个比较危险，最好是有个类似拨码开关的东西能做到这个目的，这样bypass掉spi之后可以从mmc启动；启动后连接clock信号即可更新spi nor flash。

## 4.5. SATA

rk3328上唯一的usb 3.0接口通过sata bridge连接22pin sata连接器，接2.5寸的机械硬盘或ssd。

usb-sata bridge芯片使用JMS578，封装无特殊要求；

JMS578芯片人民币4元/pcs含税，MOQ是490颗/包装；

裕顺荣科技（深圳）有限公司
联络人：吴凯磷
Tel&FAX：86-755-21008626
Mobile：13802906050
E-mail：pmsales@dgysr.com
QQ：3369643001
ADD：深圳市龙华新区和平路清湖地铁A出口OTO创客中心A栋515号

sata接口使用22pin的combo连接器（15 pin power + 7 pin signal，其中15 pin power不提供12V输出），选型和供应商待定；

## 4.6. WiFi

RK3328的唯一USB 2.0 Host接口接WiFi模块。

Wifi模块使用欧智通的6222D-UUB，内置ipex/u.fl接口；

联系人：柴振涛，13902963647，tommychai@fn-link.com；

WiFi模块上有一个enable信号引脚；需要接MPU，具体GPIO待定。

## 4.7. BLE

使用TI CC2640R2，参考设计为官方的CC2640R2 Launchpad。

+ datasheet: http://www.ti.com/tool/LAUNCHXL-CC2640R2
+ technical reference manual (TRM): http://www.ti.com/lit/ug/swcu117h/swcu117h.pdf

该芯片输出单端射频信号，需搭配陶瓷贴片天线并调试匹配电路。

该芯片有多种封装，Launchpad上使用的是VQFN48封装，7x7mm，是GPIO最多的封装，确定沿用该封装。

该芯片有小包装。

我们sourcing到的供应商为新晔（和之前利尔达的TI低功耗芯片部分合并了），他们一直做TI，但价格未必是最好的，仅供推荐：

徐成功：18116762923

BLE的设计思路是在固件正常工作时它和MPU是独立系统，而不是受控设备。实现以下功能：

### 4.7.1. BLE供电

BLE需要独立供电，不使用RK3328的PMU的sys输出；要求在Type C接口上电时该芯片即可供电。

### 4.7.2. BLE需能控制PMU的Power开关

BLE芯片可能用于在系统停电后重新上电时自动启动系统。它需要有一路GPIO工作在open drain模式下，逻辑上和PMU的Power开关OR在一起。如果Power开关是5V上拉这里应该需要电平转换。

使用DIO21实现该功能。

### 4.7.3. BLE需能感知PMU的sys power输出

目的是知道MPU子系统是否处于上电状态。暂定使用DIO15。

### 4.7.4. MPU与BLE芯片的通讯

MPU与BLE芯片的通讯使用4线uart接口。

Launchpad的设计上，信号定义如下：

+ TXD - DIO3
+ RXD - DIO2
+ CTS - DIO19
+ RTS - DIO18

DIO编号和引脚编号的对应关系请参阅datasheet 4.1 Pin Diagram – RGZ Package的图示。

MPU一侧，Pi 2 Bus接口的如下引脚作为UART接口。

Pi2 Bus Pin | MPU Pin Name | Uart Signal
--|--|--
15|GPIO3_A4|UART1_TX
16|GPIO3_A5|UART1_RTS
18|GPIO3_A6|UART1_RX
22|GPIO3_A7|UART1_CTS

### 4.7.5. BLE控制LED灯

3色LED灯使用DIO7, DIO8, DIO9控制，active high逻辑，即DIO高电平时灯亮。

### 4.7.6. MPU对BLE芯片的控制

MPU可以直接升级BLE芯片的固件。

除UART通讯之外，MPU一侧需要两个GPIO控制BLE芯片，一个接BLE芯片的Reset，另一个接`DIO13`，用于MPU的系统程序可以更新BLE芯片的ROM。

该功能在cc2640r2的文档中称bootloadr backdoor，见cc2640r2 trm 8.1 Bootloader Functionality；该信号为Open Drain，在启动监测的瞬间有内置上拉，外部可再配置一个阻值较大的上拉（>10K）。

MPU一侧也需要两个Open Drain信号，用哪个待定，需考虑驱动支持。

### 4.7.7. BLE的JTAG接口

BLE的JTAG接口与Launchpad一致，需接触TMS, TCK TDO, TDI, Reset，Power/GND，接口和TI开发板保持一致，这个是TI的Wireless MCU标准debug接口。

### 4.7.8. BLE小结

cc2640r2本身是一个功能丰富和灵活的芯片；abel项目只用到很少一部分；封装选型使用的7x7封装很可能不是必要的，实际使用的IO很少，但是团队资源有限没时间详细研究该芯片的使用，只要采购上没有很大问题就基本照搬Launchpad的设计，这样开发调试最方便。

DIO使用汇总：

DIO | 模式 | 信号/连接 | 说明
--|--|--|--
DIO21 | Open Drain | PMU的Power Key | 可设置自动启动MPU子系统的逻辑
DIO15 | Input, pull down | PMU的sys输出 | 可检测MPU子系统的上电状态
DIO3 | Output | UART TX
DIO2 | Input | UART RX
DIO19 | Input | UART CTS
DIO18 | Output | UART RTS
DIO7 | Output | Red LED
DIO8 | Output | Green LED
DIO9 | Output | Blue LED
DIO13 | Input | MPU GPIO| bootloader backdoor

## 4.8. atecc安全芯片

最终量产可能使用atecc508a或者608a，我们最初从供应商得到的信息是两者高度兼容，实际开发情况发现两者硬件完全兼容，软件有一些差异，目前基于官方demo软件调试通过的是**atecc508a**，不是608a。

封装方面：

我们拿到的样片是soic8 150mil的窄封装，之前文档所述的是UDFN8封装，对我们来说都可以；由硬件工程师决定。

需要提醒的是这颗芯片的外观不容易分辨1脚，soic的1脚侧顶部有斜切但是无标识，官方手册上说作为安全芯片故意混淆了芯片的mark，所以请注意贴片时不要贴反了。UDFN我没有见过实际芯片。

供应商：

我们开发阶段联系了microchip，推荐的供应商是威健。
联系人：白经理，18916579691

Pi2 Bus Pin | MPU Pin Name | Uart Signal
--|--|--
27 | GPIO2_A4 | I2C1_SDA
28 | GPIO2_A5 | I2C1_SCL

## 4.9. MPU的console接口

使用UART2作为Linux console，用于调试。

板上需要有2.54mm间距的孔，可焊接标准排公排目，单排双排均可。

Pi2 Bus Pin | MPU Pin Name | Uart Signal
--|--|--
8 | GPIO2_A0 | UART2_TX
10 | GPIO2_A1 | UART2_RX

## 4.10. MPU信号汇总

+ mmc
+ spi nor flash
+ usb 3 -> usb sata bridge -> hdd or ssd
+ usb 2 host -> wifi module
+ usb 2 otg -> type-c connector
+ uart1 -> ble
+ uart2 -> console/debug
+ i2c1 -> atecc
+ gpio
    + wifi enable (待定)
    + ble reset (待定)
    + ble backdoor (待定)

ROCK64的GPIO有个第三方的详细文档，包括Linux下使用的：

http://synfare.com/599N105E/hwdocs/rock64/index.html

我们试过用作GPIO的Pin包括Pi2 Bus上的Pin 3/5/26在Rock64/armbian的缺省配置下可用于GPIO，输入输出均可，缺省pull-down或无pull-up/pull-down；Pin 7也可以，缺省输入是pull-up的；没仔细看这些pull-up/pull-down是原理图上有外部上下拉电阻还是芯片本身支持在dtb里配置的。

## 4.11. 其他

目前并不详细了解Power Button的行为。

设备上只有这一个Button，有些场景下也需要它是一个User Button，理想的情况是它有GPIO驱动，而且能有edge trigger逻辑，polling可能会错过用户的短按行为，连续长按（hold）是可以polling的；

这个临滴的工程师如果熟悉可以说明一下，不行就去问瑞芯微。

最不济的办法是，BLE芯片也有这个信号，可以让BLE芯片向MPU子系统发送Power Button Down, Power Button Up等消息，也是个办法，是最后没办法的办法。

最后，type-c的连接器要选一个结实的。应该不需要沉板，中心高度和侧键中心高度尽量一致。


