BSP Notes

### 烧录工具

使用etcher工具烧录镜像至SD卡 [etcher](https://etcher.io/)

### 官方 Armbian 镜像

使用默认 Armbian Xenial 镜像，下载地址 [rock64](https://www.armbian.com/rock64/)

+ 该镜像默认root密码为1234，首次ssh登陆会要求重置密码，并添加用户，如winsuc(passwd: wisnuc)


### 编译Armbian

+ 推荐编译环境的配置：4G内存，SSD，四核CPU，20G以上硬盘空间

+ VirtualBox、Docker或者native环境的Ubuntu Bionic 18.04 x64系统

+ 管理员权限（sudo或root）

+ 执行代码

```bash
apt-get -y install git
git clone https://github.com/armbian/build
cd build
./compile.sh
```

注意编译所在环境不要有空格。首次编译过程需要下载大量内容，可能耗时数小时，之后的编译可在10min之内。

编译过程会提示选择编译类型、板子型号，内核分支及OS版本，具体可看[Build-Options](https://docs.armbian.com/Developer-Guide_Build-Options/)

目前分别选择

```
Full OS image for flashing,
Do not change the kernek configuration,
rock64,
xenial Ubuntu Xenial 16.04 LTS,
Image with console interface
```

亦可以通过配置文件config-default.conf自动设置，其中tag lock 对应commit `c63b212`

```
# Read build script documentation http://www.armbian.com/using-armbian-tools/
# for detailed explanation of these options and for additional options not listed here

KERNEL_ONLY="no"                  # leave empty to select each time, set to "yes" or "no" to skip dialog prompt
KERNEL_CONFIGURE="no"             # leave empty to select each time, set to "yes" or "no" to skip dialog prompt
CLEAN_LEVEL="make,debs,oldcache"  # comma-separated list of clean targets: "make" = make clean for selected kernel and u-boot,
                                  # "debs" = delete packages in "./output/debs" for current branch and family,
                                  # "alldebs" = delete all packages in "./output/debs", "images" = delete "./output/images",
                                  # "cache" = delete "./output/cache", "sources" = delete "./sources"
                                  # "oldcache" = remove old cached rootfs except for the newest 6 files

DEST_LANG="en_US.UTF-8"           # sl_SI.UTF-8, en_US.UTF-8

# advanced
KERNEL_KEEP_CONFIG="no"           # do not overwrite kernel config before compilation
EXTERNAL="yes"                    # build and install extra applications and drivers
EXTERNAL_NEW="prebuilt"           # compile and install or install prebuilt additional packages
CREATE_PATCHES="no"               # wait that you make changes to uboot and kernel source and creates patches
BUILD_ALL="no"                    # cycle through available boards and make images or kernel/u-boot packages.
                                  # set KERNEL_ONLY to "yes" or "no" to build all packages/all images

BSPFREEZE="no"                    # freeze armbian packages (u-boot, kernel, dtb)
INSTALL_HEADERS="yes"             # install kernel headers package
LIB_TAG="lock"                    # change to "branchname" to use any branch currently available.
CARD_DEVICE=""                    # device name /dev/sdx of your SD card to burn directly to the card when done
BOARD="rock64"                    # build for board rock64
RELEASE="xenial"                  # build xenial Ubuntu Xenial 16.04 LTS
BUILD_DESKTOP="no"                # Image with console interface
```

### 配置wifi驱动

```bash
# 查看已有驱动
find /lib -name *8188*

# 启动wifi驱动
modprobe -f 8188eu

# 查看驱动状态
dmesg
```

### 编译wifi驱动

参考[How to build a wireless driver](https://docs.armbian.com/User-Guide_Advanced-Features/#how-to-build-a-wireless-driver)

```bash
# build system with INSTALL_HEADERS="yes"

# make recordmcount, important !!
cd /usr/src/linux-headers-4.4.138-rk3328/scripts
make recordmcount

# build driver 88x2bu
cd
cd EW-7822ULC_Linux_Driver_1.0.1.6/
make

# Load driver for test
insmod 88x2bu.ko

# check driver
lsmod

# Plug the USB wireless adaptor and issue a command:
ifconfig
iwconfig wlx74da38d18b16

# Check which wireless stations / routers are in range
iwlist wlx74da38d18b16 scan | grep ESSID

# install driver
make install
```


### 设置自动连接WIFI

添加以下内容至 /etc/network/interfaces，其中wireless-ssid，wireless-key即所要连接的wifi和密码

```
auto wlx74da38d18b16
iface wlx74da38d18b16 inet dhcp
wireless-ssid SSID_Name
wireless-key XXXXX
```

### 安装node

需要下载arm v8版本的[node](https://nodejs.org/dist/v8.11.3/node-v8.11.3-linux-arm64.tar.xz)

```bash
curl -O https://nodejs.org/dist/v8.11.3/node-v8.11.3-linux-arm64.tar.xz
```

### 其它相关命令

```bash
# 通过ARP协议获取到的网络上邻居主机的IP地址，用户获取板子的ip（也可以通过查看路由器连接设备的方式）
arp

# 查看本机器linux内核版本
cat /proc/version

# 查看linux版本
lsb_release -a

# 查看usb设备
lsusb

# 显示内核缓冲区系统控制信息
dmesg
```
