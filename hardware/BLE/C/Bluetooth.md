# 编译SPP BLE SERVER

### 需求组件

+ SDK v1.5 [simplelink_cc2640r2_sdk_1_50_00_58](http://www.ti.com/tool/SIMPLELINK-CC2640R2-SDK)
+ SDK example pack v1.5 [simplelink_cc2640r2_sdk_ble_example_pack_1_50_00_62](http://www.ti.com/tool/SIMPLELINK-CC2640R2-SDK)
+ BLE extra example pack v1.5 from github [ble_examples-simplelink_sdk-1.50](https://github.com/ti-simplelink/ble_examples/tree/simplelink_sdk-1.50)
+ Code Composer Studio [ccs_setup_8.1.0.00011](http://www.ti.com/tool/ccstudio)

### 安装程序

+ 安装`Code Composer Studio`、`SDK`、`SDK example`至`C:/ti`文件夹
+ 将`BLE extra example pack`拷贝至`C:\ti\simplelink_cc2640r2_sdk_ble_example_pack_1_50_00_62`
+ 启动`Code Composer Studio`, `File` -> `Import` -> `Code Composer Studio` -> `CCS Projects` -> `C:\ti\simplelink_cc2640r2_sdk_ble_example_pack_1_50_00_62\examples\rtos\CC2640R2_LAUNCHXL\bleapps\spp_ble_server`
+ USB连接板子到电脑（windows），电脑端使用putty串口连接到COM口，参考[Project Zero](http://dev.ti.com/tirex/#/DevTool/CC2640R2%20LaunchPad/?link=Software%2FSimpleLink%20CC2640R2%20SDK%2FSimpleLink%20Academy%2FBluetooth%204.2%2FFundamentals)的说明
+ Build the projects and Flash the device

### Bootloader settings

in `source/ti/devices/cc26x0r2/startup_files/ccfg.c`

```C
#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE         0xC5       // Enable ROM boot loader
#define SET_CCFG_BL_CONFIG_BL_LEVEL                     1        // Active high to open boot loader backdoor
#define SET_CCFG_BL_CONFIG_BL_PIN_NUMBER               13       // DIO number for boot loader backdoor
#define SET_CCFG_BL_CONFIG_BL_ENABLE                 0xC5       // Enabled boot
```

### Limited discoverable mode
```C
// Limited discoverable mode advertises for 30.72s, and then stops
// General discoverable mode advertises indefinitely
// #define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_LIMITED

// ...

// Set advertising interval
uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;
GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInt);
GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInt);
// GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInt);
// GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInt);
GAP_SetParamValue(TGAP_LIM_ADV_TIMEOUT, 30); // Seconds
```