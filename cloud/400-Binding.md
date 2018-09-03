# User-Device Ownership 

User-Device Ownership是云所有的资源；

User-Device Relationship使用2-tuple描述；

```
DEVICE_ID
USER_ID
```

# Device-User Sharing

Device-User Sharing是云所有的资源；

Device-User Sharing使用2-tuple描述；

```
DEVICE_ID
USER_ID
```

# 初始化

初始化是一个复合过程，同时完成3个业务目的：

1. 对于云，初始化是创建User-Device Ownership的过程；该过程向用户授权，同时要求设备提供物理接触证据，即完成一个跨设备的op-session；
    + App负责发起该请求，完成session的第一步；
    + NAS负责完成session的第二步；
2. 对于设备而言，需要用户提供WiFi配置；
3. 对于App而言，这一步同时获得设备发放的证书，用于未来的端到端操作签名；


## 过程

1. 用户登录，App获得用户身份；
2. App搜索到周围的可绑定设备；获得如下信息：
    + 设备的Id
    + 设备的WiFi网络情况
3. 用户选择可绑定设备
    + 如果设备已经和手机连接在同一WiFi网络，下一步；
    + 否则要求用户输入手机所使用的WiFi网络的密码；
4. 用户确认后，App向云端请求绑定该设备：
    1. 提供设备id；
    2. 获得操作会话（op-session）描述，包含：
        + session: id (random)；
        + user: id；
        + op: binding；
        + timestamp: 
        + csig: cloud signature；
5. App自己生成一对公密钥；
6. App向设备发送请求，包含：
    1. op-session数据
    2. awsalb cookie
    3. wifi的ssid, bssid, password
    4. CSR (certificate signing request)
7. 设备上执行如下操作：
    1. 连接指定的WiFi，包括获取IP地址，如果失败，返回错误；
    2. 连接云服务，如果失败，返回错误；
    3. 要求用户按下按键；
    4. 向云端发起请求，包含:
        1. op-session数据；
        2. nsig: 对该数据的签名（表示确认物理接触）
    5. 如果成功
        1. 设备记录WiFi配置，主人Id，
        2. 向App签发证书（CRT）
8. 客户端应成对保存证书和密钥，密钥必须采取安全存储方式，未来用于端到端操作签名；



