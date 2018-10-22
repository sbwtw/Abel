# Microchip Zero Touch Demo

官方的zero touch采用3级证书方式：root ca -> signer -> device；其中root ca和signer的证书与密钥生成、csr生成和证书签署均采用python脚本实现；

device证书签署未使用openssl，而是使用cryptograph库，原因可能是需要保证证书格式和预定义模板格式一致。

zero touch demo的mcu firmware提供了3个预置模板，分别对应signer cert，device cert，和device csr。

完整zero touch demo流程如下：

1. 生成ca和signer证书，并上传至aws；
2. provision ecc芯片，生成device key；
3. mcu根据csr模板生成device csr；
4. 使用python cryptography的certificate builder创建device cert；
5. 把signer和device证书发送给mcu；mcu把证书存入ecc芯片，均使用模板转换；

生成device cert的细节：

1. serial number为16 bytes；使用device的public key和编码的时间作为seed，取hash；
2. serial number的最高两个bit被处理掉，保证der格式下不会改变字串长度；
3. subject key id从device的public key导出；
4. authority key id从signer的public key导出；


# Abel Zero Touch

在Abel项目中采用两级证书的做法；在量产时使用一台linux电脑承担signer责任；其该电脑及其运行的服务程序均称为`Provisioning Server`。

## Provisioning Server

### 硬件

Provisiong的硬件需求包括：

1. 一部有以太网的电脑，需要运行Linux；
2. 一个路由器；

在生产环境中需要有Internet接入方式；可以是电脑带有wifi，通过wifi接入Internet，也可以使用usb接口的移动上网卡直接接入互联网。

任何能运行Linux的电脑硬件均可作为Provisioning Server；考虑产线操作应该具有键盘、鼠标和显示器，推荐使用笔记本电脑。

### 系统

1. 电脑使用Ubuntu 16.04桌面版本；
2. 电脑预装node.js环境，10.x版本；
3. 电脑预装provisioning server服务软件；
4. 需安装有openssl（缺省有）

### 部署

```

\|/
 |
 | 
 ----（路由器）WAN口 <-----> 以太网口（电脑） ------> 互联网  

```

路由器的WAN口与电脑的Ethernet接口直连；

路由器工作在仅提供AP功能，不提供路由功能的状态（也没有DHCP服务）；

wifi的ssid name为`wisnuc-provisioning`，密码同ssid name；ssid name和password都会hard-code在设备代码中，设备在初始化时尝试连接该wifi并开始初始化工作；

电脑的以太网口工作在分享网络模式（sharing to other computer）下，自带DHCP服务，缺省ip地址为`10.42.0.1`，该ip地址同样hard-code在代码中；

provisioning server软件需要命令行启动，启动后会提示需要访问云服务的用户名密码；用户输入用户名和密码后，provisioning server开始工作。

## Provisioning过程设计

Abel采用两级证书方式，只有ca和device证书，没有中间人（intermediate）证书。在本文档中使用`signer`一词表述ca的证书和密钥。

signer证书和密钥由provisioning server负责创建；provisioning server在每次（服务）启动时即时生成signer证书和密钥，要求密钥仅存在于程序内存中，不得持久化存储或者在网络上传播，如果服务程序退出则抛弃证书和密钥，下次服务启动时重新生成。

1. signer证书使用ec算法，使用sha256算法签名；
2. signer证书的组织名称（O）为`wisnuc`；
3. signer证书的常用名称（CN）为`aws signer`
4. 仅有O和CN两个属性
5. 必须有x509v3扩展的subject key identifer和authority key identifier；作为自签名证书两者应该一致，必须用x509v3扩展的basic constraint标注为ca证书；

使用openssl命令生成的ca证书自动满足上述要求4。

provisioning server的生成证书和密钥后需要上传至云，但是不在provisioning server上部署和登录aws cli；provisioning server所需的信息，例如生成verification证书的CN，通过api访问获得。云端为provisioning server提供专门的https服务，使用用户名和密码登录。

provisioning server在成功递交证书后，可以开始签名服务。

第一步：设备向provisioning server提交如下信息：

1. atecc芯片的硬件序列号和版本号
2. csr

其中csr中使用的组织名称（O）为`wisnuc`，常用名（CN）为`winas`。

第二步：provisioning server签署该csr，要求如下：

1. 提供serial number（格式待定，不超过20 bytes
    1. 要具有随机性，因为可能出现单一设备申请过程遇到失败后重复申请的状态，可仿照microchip的设计加入随机时间；
2. 提供x509v3扩展的subject key identifier和authority key identifier；其中subject key identifier为hash，authority key identifier为keyid；
3. 证书长度最大不超过414字节；

第三步：provisioning server将签署的设备证书和signer证书一起返回给设备；

1. 设备需检查证书有效性，可使用`openssl verify`
2. 设备需检查证书内的公钥正确，可使用openssl导出公钥检查；
3. 设备使用这两个证书连接amazon aws iot，实现JITR过程；
    - 如果设备遇到失败，应向provision server报告错误，并最多重试3次，如果重试3次失败，设备应通过指示灯显示错误状态并停止重试；只有reboot可以重新开始provisioning过程；
4. 如果JITR成功，设备应将设备证书存入ecc芯片slot 8，并读取一次比对验证；
5. 最后设备lock住slot 0，通过指示灯告知provisioning成功完成；

slot 8的存储方式：

slot 8完整存储一个证书，byte 0和byte 1用于保存证书格式和长度，证书存储从byte 2开始，最大长度为414字节。

最初的两个字节定义如下：

```
Byte 0
    bit 1-7 均为0
    bit 0
Byte 1
    bit 0-7
```

Byte 0的bit 0和Byte 1的所有bit构成一个整数，为证书长度。

# 设备流程与状态

1. preset aws config
2. lock config zone
3. gen key
4. gen csr
5. sign csr
6. jitr
7. save cert
8. lock data zone

ecc芯片状态

1. 地址为C0
    - config已锁定（失败）
    - config未锁定，检查或写入config，next
2. 地址为B0
    - config



# 总结

1. 采用两级证书，provisioning server充当signer角色，不保存密钥；
2. 仅在设备生成csr时使用证书模板，这样provisioning server可以方便的使用openssl命令签署证书，主要满足证书长度要求即可；
3. 直接保存完整设备证书，不使用模板；

# 问题

在Microchip的官方设计中使用了key identifier作为aws iot上lambda创建thing的thing name；如果要采用这个做法，则每次错误重试的时候都需要重新生成device key。

