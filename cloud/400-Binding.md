# User-Device Ownership 

User-Device Ownership是云所有的资源（User和Device都是云资源）。

User-Device Relationship使用

```
USER_ID
Device_ID
```

限制：

# Binding

Binding是创建User-Device Ownership的过程。

## 用户过程

1. 用户使用App登录，获取身份；
2. 用户搜索到周围的设备（未配置）；
3. 用户选择设备，选择WiFi的SSID，输入WiFi密码，发送给设备；
4. 设备上做如下操作：
    + 连接指定的WiFi，包括获取IP地址，如果失败，返回错误；
    + 连接云服务，如果失败，返回错误；
    + 请求绑定，如果失败，返回错误，不保存任何信息；
    + 如果成功，记录所有信息，返回成功；
5. 客户端请求Token，开始访问；




