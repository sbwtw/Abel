Project Abel

该文件为保密文件。如果你不是上海闻上信息科技有限公司或上海临滴的相关项目

Copyright 上海闻上信息科技有限公司 2018

<!-- TOC -->

- [1. 版本](#1-版本)
- [2. 项目简介](#2-项目简介)
- [3. 系统概述](#3-系统概述)
    - [3.1. 硬件规格](#31-硬件规格)
    - [3.2. 通讯](#32-通讯)
    - [3.3. 软件规格](#33-软件规格)
    - [3.4. 参考设计](#34-参考设计)
- [4. 详细设计](#4-详细设计)
    - [4.1. 接口](#41-接口)
        - [4.1.1. 需要删除的外设或接口](#411-需要删除的外设或接口)
        - [4.1.2. 保留和增加的外设或接口](#412-保留和增加的外设或接口)
    - [4.2. 电源设计](#42-电源设计)
        - [4.2.1. 功率估算](#421-功率估算)
        - [4.2.2. 断电保护](#422-断电保护)
        - [4.2.3. Super Capacitor](#423-super-capacitor)
        - [4.2.4. 能量和时间估算](#424-能量和时间估算)
        - [4.2.5. efuse & Reverse Blocking](#425-efuse--reverse-blocking)
        - [4.2.6. USB Wifi Power](#426-usb-wifi-power)
        - [4.2.7. SuperCapacitor Charging & Discharging](#427-supercapacitor-charging--discharging)
        - [4.2.8. SSD/HDD 5V](#428-ssdhdd-5v)
        - [4.2.9. 小结](#429-小结)
    - [4.3. MMC卡座](#43-mmc卡座)
    - [4.4. SPI NOR Flash](#44-spi-nor-flash)
    - [4.5. SATA](#45-sata)
    - [4.6. WiFi](#46-wifi)
    - [4.7. BLE](#47-ble)
        - [4.7.1. BLE供电](#471-ble供电)
        - [4.7.2. MPU与BLE芯片的通讯](#472-mpu与ble芯片的通讯)
        - [4.7.3. BLE控制LED灯](#473-ble控制led灯)
        - [4.7.4. BLE检测用户按键](#474-ble检测用户按键)
        - [4.7.5. MPU对BLE芯片的控制](#475-mpu对ble芯片的控制)
        - [4.7.6. BLE的JTAG接口](#476-ble的jtag接口)
        - [4.7.7. DIO使用汇总](#477-dio使用汇总)
    - [4.8. atecc安全芯片](#48-atecc安全芯片)
    - [4.9. LED](#49-led)
    - [4.10. RTC](#410-rtc)
    - [4.11. MPU的console接口](#411-mpu的console接口)
    - [4.12. MPU](#412-mpu)
        - [4.12.1. 信号汇总](#4121-信号汇总)
        - [4.12.2. RK3328 IO Pad](#4122-rk3328-io-pad)
        - [4.12.3. GPIO对照表](#4123-gpio对照表)
        - [4.12.4. 信号说明](#4124-信号说明)
    - [4.13. Type-C连接器](#413-type-c连接器)
- [5. 量产镜像和产线测试](#5-量产镜像和产线测试)
    - [5.1. 量产镜像说明](#51-量产镜像说明)
    - [5.2. 预置（Provisioning）与接口测试](#52-预置provisioning与接口测试)

<!-- /TOC -->

# 1. 版本

闻上马天夫 lewis.ma@winsuntech.cn 微信 unwiredgrain

- 20181015 draft
- 20181025 rc1
    - 增加电源设计要求和讨论
    - 增加超级电容后备电源
    - 修改BLE，LED，和按键的设计要求
    - 增加RTC
    - 增加工厂测试的初步描述
- 20181025 rc2
    - 增加CDA电容
- 20181030 rc3
    - 增加korchip电容，增加MPU GPIO详细定义

# 2. 项目简介

Abel项目是一款家用NAS设备；提供云盘、照片备份等功能；用户可以远程访问设备中存储的内容，也可以把内容分享给其他用户；

该设备内置身份验证硬件（atecc芯片），与闻上云服务和云账户系统紧密集成；

# 3. 系统概述

设备外观比移动硬盘略大，外观象小型家用路由器，有两根外置WiFi天线；

设备正面无灯或按键；

设备背面有一个按键和一个USB 2.0 Type-C接口；Type-C接口用于设备供电，是设备的唯一供电方式，供电规格为5V/3A；按键为用户按键，非电源按键，设备不提供开关机按键；按键半透光，同时提供LED显示；

设备内部有2.5寸SSD，也可以支持2.5寸笔记本用机械硬盘（HDD）；使用SSD时，设备断电时提供数据回写保护，使用HDD时目前设计不能做到完善的断电数据保护，受限于设计选择的后备供电容量。

## 3.1. 硬件规格

1. 基于RK3328平台，配套RK805-1电源管理芯片（PMU）
2. 2G memory (LPDDR3, DDR3/DDR3L，DDR4)
3. 64Mbit SPI Nor flash 
4. WiFi 802.11ac
5. Bluetooth LE
6. USB 2.0 Type-C接口 x 1，固定为device模式，也是系统唯一的供电接口
7. 1个按键
8. 双色LED，外观上从按键透光
9. 内部使用超级电容（Super Capacitor）提供关机时系统供电
10. 板载RTC电路和RTC后备电池

## 3.2. 通讯

1. 无线
    - WiFi 802.11ac AC1200规格，其中5G频段为2T2R MIMO，866Mbps；
    - Bluetooth LE 4.2/5
2. 有线
    - USB 2.0 Type-C，工作在Device模式；软件计划在未来支持MTP或CDC/RNDIS模式；

## 3.3. 软件规格

1. Linux内核4.4（rockchip官方）或mainline（社区）
2. Ubuntu base 18.04 / arm64版
3. 闻上自研NAS服务器软件

## 3.4. 参考设计

硬件参考设计使用Pine64社区的ROCK64板：

+ 产品页：https://www.pine64.org/?page_id=7147
+ Pine64官方资源：https://www.pine64.org/?page_id=7175
+ Wiki：http://wiki.pine64.org/index.php/ROCK64_Main_Page
+ Rock64板载接口信号定义：http://files.pine64.org/doc/rock64/ROCK64_Pi-2%20_and_Pi_P5+_Bus.pdf

从上述资源中可以获得ROCK64的原理图。

# 4. 详细设计

## 4.1. 接口

### 4.1.1. 需要删除的外设或接口

以下外部设备从ROCK64设计中删除：
+ 以太网
+ 音频
+ HDMI
+ 红外
+ USB 3连接器
+ 原有两路USB 2接口Type A连接器删除，host一路板载接wifi，otg一路接Type C

### 4.1.2. 保留和增加的外设或接口

+ mmc卡座保留，位于设备内部，仅用于调试和维修；
+ USB 3通过USB-SATA bridge芯片接SATA接口，板载22pin SATA连接器；
+ USB 2 Host接WiFi模块；
+ USB 2 OTG接Type C接口，固定为Device模式；
+ I2C1（Pi-BUS的Pin 27/28）接atecc安全芯片；
+ UART1（Pi-BUS的Pin 15/16/18/22）接蓝牙BLE芯片；
+ UART2为调试接口，板上留2.54mm间距过孔焊盘，无连接器；

## 4.2. 电源设计

### 4.2.1. 功率估算

系统供电规格为5V/3A。

Rock64整板在服务器模式下（即HDMI，GPU，音频都没有负荷，但Eth有）的极限电流是1.1A，普通负荷下800mA以内；SSD的极限功耗是2800mW，即560mA；WiFi模块（RTL8822BU）功耗没有官方数据，一般基于RTL的类似方案（RTL8812AU）的极限功耗是5V/450mA；sata bridge的极限功耗是1.2V 250mA，折合5V并考虑效率损失不超过60mA，ble功耗可忽略不计。

以上估算，系统的极限功耗是5V，2.2A水平，实际使用应该远低于该水平。

### 4.2.2. 断电保护

电源系统设计的重要目标之一是考虑系统在USB断电时能够完成正常的软件关机过程，至少能够保证文件系统的内核缓冲区数据回写SSD，保证数据不会丢失；该问题是此类产品的设计特点，也是用户不会高频遇到、但是在使用过程中肯定会遇到的痛点，电源故障导致大量nas设备用户的数据损失。

原计划系统设计目标是断电后可以提供8s以上的供电；实际上受到器件选型和成本限制可能只能做到3-4s的水平；在该限制下断电时完成SSD数据回写容易保证，如果使用HDD则很难保证；无论SSD/HDD，都无法保证完成完整的软件关机流程，只能做到文件系统数据正确。

```
                --------------------------------------------> USB WiFi Power
                |
                |                      -----------(boost)---> SSD 5V/1.2A
                |                      |             |
                |                      |             ----EN
Vusb            | (1)          Vmain   |
(usb)-->(efuse)---___----------------------->(PMU)----------> System
                   |        |          ^
             EN-----        v          |
                        (charger) (discharger)
                            |          ^
                            v          |
                          (supercapacitor)
```

### 4.2.3. Super Capacitor

在设计中加入超级电容作为后备供电。使用超级电容主要是考虑比锂电池更加安全，尤其是长时间高温的工作情况。

超级电容的单体电压一般为2.5-2.7V，串联使用时和锂电池一样存在平衡问题，包括两方面：

1. 串联的超级电容容量必须较为精确的一致，否则容值低的器件会获得较高的电压，容易超过极限电压损毁或显著降低使用寿命；
2. 串联的超级电容在充电时，如果ESR<sub>DC</sub>相差较大也可能导致分压不一致，获得高分压的器件容易超过极限电压损毁或显著降低使用寿命；

情况1只能由器件原厂封装模组实现，市售超级电容的容值误差一般在+/-25%范围，最高偏差值为最低偏差值的1.5倍，不可能自己分拣器件或者超配损失容量；

情况2对于2-PIN模组由模组内部解决；3-PIN模组需要工程师设计外部电路；3-PIN模组由于使用不便已经在市场上消失；

2-PIN、双2.7V单体串联的5V模组在市场上最为普遍，极限电压在5.4V左右，通常可达安培级放电（需与原厂确认）；可选物料包括：

**Korchip DRML5R4505**

5V/5F，已联系经销商；规格符合要求，价格在7元以内，含税；

**CDA的CHP系列**

CDA是台湾的电容器厂商，其超级电容产品在淘宝上销售很多；其高容量产品线是CHP系列：

http://www.cda-cap.com/products/664.html

指标上，CDA产品的ESR<sub>DC</sub>高于Maxwell/nesscap，Tokin，KEMET等厂商的同类产品；考虑到负载的电阻，如果5V 1-2A的话，阻值在5Ω-2.5Ω，100-200mΩ的ESR<sub>DC</sub>尚在可接受范围内；CDA CHP系列的放电能力在可用范围；

**Maxwell/Nesscap**

Maxwell的5V模组（原nesscap产品线，被Maxwell收购）；该模组有详细Datasheet，放电能力极佳（极限5.1A）；但容值只有1.5F和2.5F两档，如果要提高需要并联使用；未能找到合适的供应商；

http://www.maxwell.com/products/ultracapacitors/5-volt-modules/documents

**其他**

有国产厂家但是感觉不是很靠谱，例如Kamcap（凯美），主要是担心产品质量，立创商城有很多Kamcap的型号，价格也还可以。

其他国际厂商包括KEMET，TOKIN等，都没问，估计价格都比较贵；松下的超级电容只有2.7V规格。

### 4.2.4. 能量和时间估算

E = 1/2 * C * V<sup>2</sup>

从5V放电到3.3V：

E = C * (5 * 5 - 3.3 * 3.3) / 2 ~= 7 * C

即对应5F是35焦耳。如果系统的功耗是5V/1A，理论上可支撑7s。

实际系统功耗在重度负荷下会超过1A，但平均负载情况下，且去除WiFi模组供电，低于1A。

### 4.2.5. efuse & Reverse Blocking

原则上要求USB输入的电源具有如下保护措施：

1. 过流**
2. 过压（OVP）**
3. 低电压（UVLO）
4. inrush/slew rate control
5. reverse blocking**

要求1必须提供，保证在系统发生故障时，能够断开供电；且fault response为latch模式，即断开供电后，只有Vusb断开并重新连接，才会再次给系统供电。

要求2对该产品而言是很难出现的场景，只有电源适配器故障才会出现；但提供该保护可以保证超级电容不会被击穿损毁；如果不能在efuse上提供该功能，也需要在超级电容的充电电路上提供。

要求3和4一般的efuse芯片都有，没有也可以。

要求5必须提供，如果电源适配器失去供电，大多数USB电源适配器都会在输出端快速放掉输出电压，这会导致超级电容快速放电。

> 容量不高的超级电容快速放电没有危险，35焦耳仅8.3卡热量，可以让1克水温升8.3摄氏度。

TI的最新的efuse系列是tps2595，5V版本OVP是5.7V，价格0.4$，在TI的产品里算是有竞争力的，供设计参考。

http://www.ti.com/product/tps2595

### 4.2.6. USB Wifi Power

USB WiFi功耗较大，设计上要求断电时尽快移除该路供电。

如果硬件实现，一种设计方式是：在图示中标注为(1)的位置放置高边开关；另外一种设计方式是关闭USB WiFi的使能引脚；用软件实现也可以。

使用硬件实现的好处是容易保证关闭动作在断电的最早时间发生；使用软件实现的好处是关闭的动作逻辑更灵活；例如供电由于触碰发生短暂失去供电（~100ms），那么软件上可以延迟100ms再关闭WiFi。

原则上倾向于软件实现，但最终设计看USB WiFi的功耗影响而定；最初的版本可以放置硬件开关，同时提供0ohm电阻bypass开关。

### 4.2.7. SuperCapacitor Charging & Discharging

设计要求：

1. 从Vmain向SuperCap充电的电流需要限流；
2. 从SuperCap向Vmain放电；
3. 充电电压高于5.2V时截止；

> 图示的放电路径是SuperCap直接向Vmain放电，在断电时Vmain从5V一直下降到PMU截止电压(～3.3V)；另一种设计方式是试图保持Vmain的5V，使用boost convert放电，这样做需要boost convert的输出要达到2A以上，成本较高，需要电感也很大。

最简单的电路一个限流电阻和一个肖特基管即可工作，但：

1. 限流电阻的功耗限制了充电电流，充电时间会很长；
2. 肖特基管的压降对于超级电容的工作方式而言有显著的能量损失；

超级电容储能和电压平方正比，仅计算相对值，5V时能量25，3.3V时能量10.89，可用能量～14.1；如果用肖特基，实际放出来的按4.75V计，平方是22.56，可用能量～11.7；11.7/14.1~=80%；所以在超级电容供电时电容电压损失0.25V，可用能量损失约20%，应该没算错。

该充放电电路本质上是正反两个Load Switch：

```
             Load Switch 1，限流
             ------------------>
Vmain -------                   ----------- SuperCap
             <------------------
             Load Switch 2，不限流
```

Load Switch 1工作在fault模式下，需要fault response模式为限流输出或自动重试（auto-retry）而不是Latch；Load Switch 2不限流。

Load Switch是常见物料，各大电源厂商都有。

TI有一个appnote是使用efuse（可看作增强型Load Switch）给SuperCap充电的：

http://www.ti.com/lit/an/slva920/slva920.pdf

murata的SuperCap产品线文档推荐了torex的XC8091

https://www.torexsemi.com/file/xc8109/XC8109.pdf

我查了一下常用于USB输出限流的On-Semi NCP380系列应该也可以用。

https://www.onsemi.jp/PowerSolutions/document/NCP380-D.PDF

NCP380有可调限流的版本，最低500mA，最高2.1A；如果选用这颗物料则充放电需要用两颗，可以用同一个IC型号，只是限流电阻设置不同。但这颗物料的MOSFET内阻是80ohm，稍微高了一点。

该问题我在TI论坛上问了一下，TI工程师推荐了另外一颗物料：TPS2595

http://www.ti.com/product/tps2595

有很多型号，其中有型号可以精确控制输出的截止电压的；这个物料不带反向截止，TI的工程师确认了只要保证EN开启，FET会保持导通，即可以利用反向电流作为放电，这样一颗物料即可。

https://e2e.ti.com/support/power-management/f/196/p/739642/2730619

如果充电电路不能精确在5.2V截止充电，可考虑用TL431系列precision shunt regulator控制Load Switch的EN引脚，是简单可行成本不高的做法；立创商城上国产或台湾的TL431仅几毛钱。

### 4.2.8. SSD/HDD 5V

在系统断电时，SSD/HDD的5V需要额外的升压电路供电。

对于SSD，最大电流假设为560mA，在Vmain下降到3.3V时升压比为1.5，即电感电流要到840mA，再考虑升压效率（～0.8）和ripple（～200mA），最终：

I<sub>L</sub> = 840 / 0.8 + 0.2 / 2 = 1.15A

频率f和电感L的乘积根据计算大概是14左右，常用的4.7uH~6.8uH电感，对应的芯片的开关频率分别是3MHz和2MHz；这个规格选择boost不算太麻烦，只是电感尺寸略大。

2.5寸HDD的情况复杂一些。

2T容量的2.5寸HDD最大启动电流为5V/1A（1-2T容量的7mm盘）或5V/1.2A（3-5T容量的15mm盘），这是在硬盘已经进入停转Idle状态后重新启动所需的电流，但在绝大多数情况这意味着系统已经无数据需要写入；在硬盘已经开始旋转后，所需的读写电流比SSD还要小一些；

在系统Vmain供电时，如果boost电路有并联的Load Switch或者boost电路本身有bypass模式，HDD的启动和读写都不会有供电问题。

在硬盘旋转时系统掉电，相比SSD，是需要工作的时间长而不是电流更大，同样数据所需写入时间会比SSD长，但是考虑到系统仅有WiFi通讯做数据传输，内核中需要回写的数据量不会太大，其写入的数据通常可以在2-3秒的时间内写入HDD。

如果掉电时系统的硬盘处于停转状态，需要启动旋转写入数据后再关闭，一般7200rpm HDD的spin-up时间在3s左右，2.5寸的5400rpm HDD时间更长一些：

```
// spin-up所需能量
E1 = 1A * 5V * 3s = 15
```
前面计算的35焦耳还需要包含系统其他部分工作所需能量，所以5F下未必能保证这种情况下的数据写入。

综上考虑：

1. SDD/HDD 5V供电仅考虑SSD的数据回写要求即可，对于HDD停转需要spin-up的情况暂不列入设计范围，即SSD 5V的设计要求是3.3V输入时有560mA的输出能力；
2. 如果boost电路没有bypass能力，考虑并联一路Load Switch，这样boost电路仅在掉电时瞬态工作，可以保证电感不会因长期工作后温升显著降低效率；

### 4.2.9. 小结

总的来说超级电容的设计有点挑战，消费电子设计中不常用，所以缺乏廉价配套IC；实际使用的参数设置和性能也只能在板子上以测量结果为准。

在软件上，避免数据损失可以用激进的方式写入SSD的数据，例如每次发生一定数量的文件系统数据写入后都立刻调用sync；或者在系统中用独立进程周期性执行sync；在这种情况下，系统供电移除后只需1-3s即可保证数据完整性；前述的断电后8s供电目标可以在实际测试结果后确定是否减少。

目前选择的5F电容，如果充电电流设计为500mA，按照简单的IT=CU估算，充满需要50s。这意味着在系统启动后但是50s之前的时间里如果发生断电，超级电容系统并不一定能保证系统的数据写入；而系统启动速度也是使用体验中追求的一个目标，越短越好，这里存在设计矛盾。

理想系统应该是：

1. 尽可能降低系统功耗和减少系统启动时间；
2. 超级电容应该在系统启动时间之内充电完成；

这些是未来系统进化时需要优化的工作；当前系统在设计上只需避免不必要的显著功耗。

> 另外一个设计心得是Analog/Linear的专用SuperCap Charger IC，虽然很贵，但确实是解决问题的。它能够自己处理balance问题，这样就能用两颗2.7V超级电容使用，物料采购容易很多。
>
> 可以估算一下worst case：
> 
> 如果用两颗同规格电容但是两者容值误差近50%的超级电容，一个1F，另一个1.5F，两者的分压比是3:2，如果1F的极限是2.7V，1.5F的分压是1.8V，串联后电压只能冲到4.5V使用，会损失大约1/3的理论可用能量。

## 4.3. MMC卡座

MMC沿用ROCK64设计，量产产品贴片，需要一个push-push连接器或者手机上用的那种半长的侧插（tf卡插入后有一半露在外面），位置在版边，正反均可；

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

在使用或维修过程中，如果需要再次烧录SPI flash，需要有一种办法在启动时屏蔽SPI信号，社区里常用的做法是用镊子短路其clock，这个比较危险，最好是有个类似拨码开关的东西能做到这个目的，这样bypass掉spi之后可以从mmc启动；启动后连接clock信号即可更新spi nor flash。

> SPI芯片可能在维修或调试阶段使用烧录器烧录，应考虑SPI的电源域不要有太大的负载，否则烧录器可能无法供电。

## 4.5. SATA

RK3328上唯一的usb 3.0接口通过sata bridge连接22pin sata连接器，接2.5寸的机械硬盘或ssd。

usb-sata bridge芯片使用JMS578，封装无特殊要求；

JMS578芯片人民币4元/pcs含税，MOQ是490颗/包装；

裕顺荣科技（深圳）有限公司
联络人：吴凯磷
Tel&FAX：86-755-21008626
Mobile：13802906050
E-mail：pmsales@dgysr.com
QQ：3369643001
ADD：深圳市龙华新区和平路清湖地铁A出口OTO创客中心A栋515号

sata接口使用22pin的combo连接器（15 pin power + 7 pin signal，其中15 pin power不提供12V输出），选型和供应商见项目其他文档；

## 4.6. WiFi

RK3328的唯一USB 2.0 Host接口接WiFi模块。

Wifi模块使用欧智通的6222D-UUB，内置ipex/u.fl接口；

联系人：柴振涛，13902963647，tommychai@fn-link.com；

WiFi模块上的WIFI_REG_ON和WIFI_WAKE_HOST信号需要接RK3328，见后面的章节。

WiFi模块上的Bluetooth不用，PCM也不用，相关引脚悬空或下拉到地。

## 4.7. BLE

使用TI CC2640R2，参考设计为官方的CC2640R2 Launchpad。

+ datasheet: http://www.ti.com/tool/LAUNCHXL-CC2640R2
+ technical reference manual (TRM): http://www.ti.com/lit/ug/swcu117h/swcu117h.pdf

该芯片输出单端射频信号，需搭配陶瓷贴片天线并调试匹配电路。

该芯片有多种封装，Launchpad上使用的是VQFN48封装，7x7mm，是GPIO最多的封装，确定沿用该封装。

该芯片有小包装。

我们sourcing到的供应商为新晔（和之前利尔达的TI低功耗芯片部分合并了），他们一直做TI，但价格不好，仅供推荐：

徐成功：18116762923

立创商城上该芯片报价是1美金出头。

### 4.7.1. BLE供电

BLE使用PMU输出的3.3V供电。

### 4.7.2. MPU与BLE芯片的通讯

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

### 4.7.3. BLE控制LED灯

BLE控制status LED，见LED部分说明。

### 4.7.4. BLE检测用户按键

使用`DIO14`，DIO14是Launchpad参考设计上`DIO14_BTN2`；

### 4.7.5. MPU对BLE芯片的控制

MPU可以直接升级BLE芯片的固件。

除UART通讯之外，MPU一侧需要两个GPIO控制BLE芯片，一个接BLE芯片的Reset，另一个接`DIO13`，用于MPU的系统程序可以更新BLE芯片的ROM。

该功能在cc2640r2的文档中称bootloadr backdoor，见cc2640r2 trm 8.1 Bootloader Functionality；该信号为Open Drain，在启动监测的瞬间有内置上拉，外部可再配置一个阻值较大的上拉（>10K）。

这两个信号分别称为BLE_RESET和BLE_BACKDOOR，见MPU部分说明。

### 4.7.6. BLE的JTAG接口

BLE的JTAG接口与Launchpad一致，需接触TMS, TCK TDO, TDI, Reset，Power/GND，接口和TI开发板保持一致，这个是TI的Wireless MCU标准debug接口。

### 4.7.7. DIO使用汇总

DIO | 模式 | 信号/连接 | Launchpad net | 说明
--|--|--|--|--
DIO2 | Input | UART RX | DIO2_RXD
DIO3 | Output | UART TX | DIO3_TXD
DIO6 | Output | Blue LED | DIO6_RLED
DIO13 | Input | MPU GPIO| DIO13_BTN1 | bootloader backdoor
DIO14 | Input | button | DIO14_BTN2
DIO18 | Output | DIO18_RTS |
DIO19 | Input | DIO19_CTS

## 4.8. atecc安全芯片

最终量产可能使用atecc508a或者608a，我们最初从供应商得到的信息是两者高度兼容，实际开发情况发现两者硬件完全兼容，软件有一些差异，目前基于官方demo软件调试通过的是**atecc508a**，不是608a。

封装方面：

我们从microchip direct网站购买的样片是soic8 150mil的窄封装，量产一般用UDFN8封装，都可以，由硬件工程师决定。

需要提醒的是这颗芯片的外观不容易分辨1脚，soic的1脚侧顶部有斜切但是无标识，官方手册上说作为安全芯片故意混淆了芯片的mark，所以需要注意贴片方向。UDFN我们没有见过实际芯片。

供应商：

开发阶段联系microchip，推荐的供应商是威健。

联系人：白经理，18916579691

Pi2 Bus Pin | MPU Pin Name | Uart Signal
--|--|--
27 | GPIO2_A4 | I2C1_SDA
28 | GPIO2_A5 | I2C1_SCL

在Rock64设计上，PMU的I2C也在I2C1上，实际调试程序未发生问题；目前软件使用linux userspace i2c访问ecc芯片已经完成。

## 4.9. LED

系统需两个LED显示，一个称status LED，用于显示系统工作状态：包括正在启动，启动完成，正在关机等等；另一个称fault LED，用于显示系统故障状态，例如网络断开等等；

理想设计是白色用于status，橙色用于fault，如果不容易找到白色和橙色的双色LED，可改为蓝色和橙色（或黄色），蓝色用于status，橙色或黄色用于fault；

status LED由BLE芯片控制，MPU通过与蓝牙的串口通讯修改其状态；status LED用BLE芯片单独控制主要是考虑在MCU里用PWM和Timer控制更方便，linux里的pwm驱动和实时性都难以保证；

fault LED由MPU直接控制；主要是考虑量产时工厂测试方便，即使BLE故障，fault LED仍可以工作；

## 4.10. RTC

RTC电路设计由临滴提供，为独立RTC系统，包括晶振、芯片和后备电池，后备电池使用标准电池座和CR2032。

RTC可以和atecc放在同一个bus上（i2c1）。

## 4.11. MPU的console接口

使用UART2作为Linux console，用于调试；板上需要有3个2.54mm间距的排孔，分别对应GND, RX, TX。

Pi2 Bus Pin | MPU Pin Name | Uart Signal
--|--|--
8 | GPIO2_A0 | UART2_TX
10 | GPIO2_A1 | UART2_RX

## 4.12. MPU

### 4.12.1. 信号汇总

+ mmc
+ spi nor flash
+ usb 3 -> usb sata bridge -> hdd or ssd
+ usb 2 host -> wifi module
+ usb 2 otg -> type-c connector
+ uart1 -> ble
+ uart2 -> console/debug
+ i2c1 -> atecc (pmu的i2c也在该bus上)
+ gpio
    + USB_WIFI_REG_ON, USB_WIFI_WAKE_HOST
    + FAULT_LED
    + BLE_RESET, BLE_BACKDOOR
    + FACTORY_MODE
    + USB_PWRDET (input，感知USB电源)

### 4.12.2. RK3328 IO Pad

Rockchip提供了io pad列表的excel文件。

所有io pad分为7个电源域：

- VDD_PMU，3.3V IO，SPDIF，MUTE，HDMI的数字信号等；
- VCCIO1，3.3V，SPDIF等不常用信号；
- VCCIO2，1.8V，emmc信号；
- VCCIO3，3.3V，mmc卡信号；
- VCCIO4，3.3V或1.8V，可配置为sdio + uart + pcm + gpio的wifi/bt combo接口，也可以配置为gmac，ROCK64上实际用的是3.3V，选择了gmac配置；
- VCCIO5，3.3V，包括uart2, i2c1, i2s1等；
- VCCIO6，3.3V，包括uart1和spi；

Abel使用VCCIO6的uart1和spi，VCCIO5的uart2和i2c1；仅使用VCCIO5中的其他io用作GPIO，不使用其他电源域的数字io。

### 4.12.3. GPIO对照表

ROCK64的GPIO有个第三方的详细文档，包括Linux下使用的：

http://synfare.com/599N105E/hwdocs/rock64/index.html

VCCIO5的GPIO入下表所示，不包含uart2, i2c1，pmu用的gpio，和ir-rx；所有GPIO均在Rock64的Pi 2 Bus和Pi 5 Bus上可以使用，方便在DVT/EVT板子出来之前可以整板调试。

- PP: push-pull
- PU: pull-up
- PD: pull-down
- HiZ: High Z，高阻

ball | 初始<br>状态<sup>1</sup> | GPIO | Pi2 | Pi5 | GPIO<br>编号 | 可用 | 初始<br>状态<sup>2</sup> | 重置<br>状态<sup>3</sup> | Abel Signal
--|--|--|--|--|--|--|--|--|--
T16 | PU | GPIO2_B4 | 26 | - | 76 | yes | PU | PU | BLE_BACKDOOR, PU
U17 | PU | GPIO2_A3 | 12 | - | 67 | no<sup>4</sup> |  
R17 | PU | GPIO2_D1 | 3 | 22 | 89 | yes | PU | PU | 
P17 | PU | GPIO2_D0 | 5 | 21 | 88 | yes | PU | PU |
N18 | PD | GPIO2_B7 | - | 10 | 79 | yes | PD | HiZ | FACTORY_MODE, IN/PD
R18 | PD | GPIO2_C2 | - | 4 | 82 | yes | PD | HiZ | FAULT_LED, PP, active high
P18 | PU | GPIO2_C1 | - | 3 | 81 | yes | PD | HiZ | BLE_RESET, OUT, PP/PD, active high
V15 | PU | GPIO2_C0 | - | 9 | 80 | yes | PD | HiZ |
N17 | PU | GPIO2_C7 | - | 5 | 87 | yes | PD | HiZ |
V18 | PU | GPIO2_C4 | - | 12 | 84 | yes | PD | HiZ | USB_WIFI_REG_ON, OUT, active high
V17 | PU | GPIO2_C5 | - | 11 | 85 | yes | PD | HiZ |
V16 | PU | GPIO2_C6 | - | 14 | 86 | yes | PD | HiZ | USB_WIFI_WAKE_HOST, IN/PD
U16 | PU | GPIO2_C3 | - | 6 | 83 | yes | HiZ | HiZ 

说明：

1. 该初始状态为rockchip的io pad excel文件所描述；
2. 该初始状态为rock64板子实测，与1不符之处可能是1文档有误，或者在uboot或内核启动时修改了gpio的设置；设计以该列所列的值为准；
3. 重置状态指设置该引脚为GPIO out之后重新设置为in，实际测试发现N18-V16引脚均回到了floating的高阻状态，而不是初始的PD，万用表测量电压在1.33V左右，软件读取的GPIO值随机为0或者1，所以硬件设计上需要在外部下拉；
4. 该引脚无法使用，在设置为OUT时固定输出高电平，原因未知；
 
### 4.12.4. 信号说明

**FACTORY_MODE**

该引脚为输入，如果外部上拉到高电平，则系统启动程序认为目前处于工厂生产模式，会执行产测程序；否则是用户模式，正常启动；

**FAULT_LED**

LED应该由三极管驱动，该引脚为基极，高电平有效（LED亮）；

**BLE_BACKDOOR**

该引脚Open Drain（在Linux里设置为输出0或者输入），外部上拉到高电平，可直连BLE的对应引脚；

**BLE_RESET**

BLE的Reset上拉到高电平；该引脚通过三极管拉低Reset信号；该引脚本身为push-pull输出，高电平有效；

**USB_WIFI_REG_ON**

该引脚为输出，控制USB WiFi的使能，高电平有效；

**USB_WIFI_WAKE_HOST**

该引脚为输入，高电平有效，用于WiFi模块唤醒MPU，实际可能用不到，MPU并不会休眠；

## 4.13. Type-C连接器

Type-C连接器要保证：

1. 3A电流没有问题；
2. 结构上尽量坚固；

# 5. 量产镜像和产线测试

## 5.1. 量产镜像说明

量产镜像包括SPI镜像和SSD镜像。

系统启动过程为从SPI镜像启动，载入uboot，uboot会枚举USB设备找到内核和rootfs文件系统。

SSD镜像实际会把SSD分为如下分区，分区表格式为MSDOS：

- 主分区1，称p分区（persistent），ext4，用于放uboot的env文件，swap文件，在系统升级时需要的
- 主分区2，称a分区，ext4, 用于放rootfs，包含kernel
- 主分区3，称b分区，ext4, 用于放rootfs，包含kernel
- 主分区4，用户数据分区，btrfs文件系统

存在两个rootfs分区，目的是使用a/b升级模式；哪个分区生效记录在p分区的uboot env文件中；

实际使用的a/b分区在系统启动后均为read-only模式挂载；

## 5.2. 预置（Provisioning）与接口测试

预置是产线初始化设备证书的过程。每个设备的atecc芯片具有唯一密钥，在量产时签发证书给该设备，存储在p分区，该证书不需要保证不丢失，作为公钥证书可以重新请求。证书必然与唯一密钥匹配。唯一密钥可用于签名，但不可用于数据加密，也不可读出。

预置过程的量产部署如下：

```
   wifi <--------------------> wifi                 Internet
   \|/                         \|/                     ^
    |                           |                      |
---------              -------------------        -----------------
|       |              |                 |  eth   |               |
| PCBA  |              | Wireless Router |--------| Proxy Server  |
|       |              |                 |        |               |
---------              -------------------        -----------------
  ^  ^                                             Linux Computer
  |  |
 SSD GPIO
```    

先决条件：

1. PCBA的SPI应预烧录或者治具烧录完成
2. 应通过某GPIO引脚通知PCBA当前为产线模式启动
3. 连接包含镜像的SSD

预置：

1. 上电后进入预置和测试程序
2. 系统首先尝试WiFi连接无线路由获取IP地址
3. 预置EC芯片证书
    1. PCBA首先初始化atecc芯片
    2. 向Proxy Server发出签发证书的请求
    3. Proxy Server向云服务器请求
4. 蓝牙
    1. 下载BLE固件到蓝牙芯片
    2. 蓝牙测试（具体方法待定）
5. RTC
    1. 根据NTP服务对时结果写入RTC
6. mmc卡
    1. 需测试人员手工插入mmc
    2. 在mmc卡上进行数据读写

如果步骤2失败，通过fault LED闪烁告知测试人员
如果步骤3-6存在失败，通过Proxy Server UI告知测试人员


