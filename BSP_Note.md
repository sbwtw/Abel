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

### 编译wifi驱动

参考[How to build a wireless driver](https://docs.armbian.com/User-Guide_Advanced-Features/#how-to-build-a-wireless-driver)

```bash
# in build dir
scp output/debs/linux-headers-rk3328_5.58_arm64.deb root@192.168.31.219:~/

# rock64 root dir
cd
git clone https://github.com/pvaret/rtl8192cu-fixes.git
cd rtl8192cu-fixes
make ARCH=arm

# Load driver for test
insmod 8192cu.ko

# Check dmesg and the last entry will be:
# usbcore: registered new interface driver rtl8192cu

# Plug the USB wireless adaptor and issue a command:
iwconfig wlan0

# Check which wireless stations / routers are in range
iwlist wlan0 scan | grep ESSID
```

### 安装node

需要下载arm v8版本的[node](https://nodejs.org/dist/v8.11.3/node-v8.11.3-linux-arm64.tar.xz)

```bash
curl -O https://nodejs.org/dist/v8.11.3/node-v8.11.3-linux-arm64.tar.xz
```

### 其它相关命令

+ 通过ARP协议获取到的网络上邻居主机的IP地址，用户获取板子的ip（也可以通过查看路由器连接设备的方式）

```bash
arp
```

+ 查看本机器linux内核版本

```bash
cat /proc/version
```

+ 查看linux版本

```bash
lsb_release -a
```
