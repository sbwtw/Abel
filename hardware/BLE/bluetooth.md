# 选型

Bluetooth选用TI CC2640R2F模块，支持Bluetooth 4.2/5

# 开发板


# 编译SPP BLE SERVER

### 需求组件

+ SDK v1.5 [simplelink_cc2640r2_sdk_1_50_00_58](http://www.ti.com/tool/SIMPLELINK-CC2640R2-SDK)
+ SDK example pack v1.5 [simplelink_cc2640r2_sdk_ble_example_pack_1_50_00_62](http://www.ti.com/tool/SIMPLELINK-CC2640R2-SDK)
+ BLE extra example pack v1.5 from github [ble_examples-simplelink_sdk-1.50](https://github.com/ti-simplelink/ble_examples/tree/simplelink_sdk-1.50)
+ Code Composer Studio [ccs_setup_8.1.0.00011](http://www.ti.com/tool/ccstudio)

### 安装程序

+ 在windows平台下安装`Code Composer Studio`、`SDK`、`SDK example`至`C:/ti`文件夹
+ 将`BLE extra example pack`拷贝至`C:\ti\simplelink_cc2640r2_sdk_ble_example_pack_1_50_00_62`

### Project Zero
+ 编译与运行Project Zero以熟悉概念和操作，具体说明见TI的[Fundamentals](http://dev.ti.com/tirex/#/DevTool/CC2640R2%20LaunchPad/?link=Software%2FSimpleLink%20CC2640R2%20SDK%2FSimpleLink%20Academy%2FBluetooth%204.2%2FFundamentals)

### 编译spp_ble_server项目

+ 启动`Code Composer Studio`, 导入`spp_ble_server`, 具体操作为`File` -> `Import` -> `Code Composer Studio` -> `CCS Projects` -> `C:\ti\simplelink_cc2640r2_sdk_ble_example_pack_1_50_00_62\examples\rtos\CC2640R2_LAUNCHXL\bleapps\spp_ble_server`
+ USB连接板子到电脑（windows），电脑端使用putty串口连接到COM口，参考[Project Zero](http://dev.ti.com/tirex/#/DevTool/CC2640R2%20LaunchPad/?link=Software%2FSimpleLink%20CC2640R2%20SDK%2FSimpleLink%20Academy%2FBluetooth%204.2%2FFundamentals)的说明
+ Build the projects and Flash the device
+ 在putty上看到以下内容即为成功
```
Hello from SPP BLE Server! With Data Length Extension support!
Advertising...
```

### convert .hex to .bin

+ 可以在CCS编译的结果中找到intelhex格式的projectName.hex文件
+ 安装python的IntelHex包
+ 使用hex2bin.py转化.hex 为.bin文件
```bash
hex2bin.py project_zero_cc2640r2lp_app.hex project_zero_cc2640r2lp_app.bin
```

### 使用cc2538-bsl.py 烧录BLE的镜像文件

+ 连接板子至电脑，出现`/dev/ttyACM0`
+ 下载cc2538-bsl.py
```
sudo apt-get install python-serial
git clone https://github.com/JelmerT/cc2538-bsl.git
cd cc2538-bsl
```
+ 擦除原有内容
```
sudo ./cc2538-bsl.py -p /dev/ttyACM0 -b 115200 -e
```
+ 写入新镜像
```
sudo ./cc2538-bsl.py -p /dev/ttyACM0 -b 115200 -w project_zero_cc2640r2lp_app.bin
```
