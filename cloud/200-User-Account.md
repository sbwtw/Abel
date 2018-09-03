# 用户账户（User Account）

用户账户包含三个基础信息：

1. Id，全局唯一身份标识
2. Password，用户身份验证（Authentication）凭据（Credential）
3. Alternative Authentication Method，其他身份验证方式

# 业务

账户资源的基础业务是用户通过某种身份验证方式完成身份验证，获取访问其他云端资源的访问凭证，一般为有时效的令牌（Token）。

账户本身也是一个资源，也有增删改查的需求，包括：

1. 修改密码（基础验证凭据）
2. 增加或删除其他身份验证方式
3. 找回密码

# 身份验证

身份验证需要平衡易用性和安全性。

现代互联网（SaaS）服务的身份验证设计不仅仅考虑用户提交的验证内容，还更广泛的考虑用户访问所使用的设备、地点、间隔访问时间等多重因素。

可用的身份验证方式包括：

+ 密码
+ 第三方帐号，例如微信
+ 电话号码 + 验证码
+ Email邮箱

选择身份验证的方式的另一个考虑因素是用户的密码恢复设计；如果设计中存在单一方式可以恢复密码，同时密码或密码加上该单一方式可以构成MFA获取修改账户内所有信息的权限，则该单一方式事实上成为Root of Trust。

设计中不强求规避单一的Root of Trust，但应规避高风险的RoT设计，并提供更高强度的方式给用户。







### 

；可以用作身份识别的方式包括：
    + 可以信赖的第三方帐号登录，例如微信；
    + 手机号码和手机验证码
    + Email和Email验证

当具有两个或两个以上的身份识别方式时，可以使用多方式验证提高账户的安全性。

在创建账户时，用户需要提供身份识别方式，基于微信或者手机验证码的身份识别方式可以即时验证，账户可以立刻生效；

基于Email的验证方式，传统上使用延时验证，会增加账户的状态；也可以阻塞用户操作，在程序中保留状态，直至用户填写了发送至Email的验证码；这样可以减少账户的状态负担。

在创建账户之后，用户对于添加、修改、和删除任何一种身份识别方式的操作，均要求用户输入密码；同时可以使用一种


这些基础信息需要在创建账户时提供；如果用户选择了手机号码+微信，则验证即时完成，账户生效；如果用户选择了手机号码+Email，则账户进入Under Verification状态。

账户管理规则包括：

1. 修改基础登录凭据，或添加、修改、删除身份识别方式，需要提供原有密码和至少一种身份识别方式；
2. 恢复登录凭据，需要提供至少两种身份识别方式；

要提供原有的基础登录平局，并使用至少一种身份识别方式；
2. 恢复基础登录凭据的方式，应使用至少两种身份识别方式；
3. 添加和删除身份识别方式的方式，提供原有的基础登录凭据并使用至少一种身份识别方式；

# 账户系统设计原则

1. 闻上账户，微信账户，和手机号码分离设计；
2. 闻上账户与微信账户之间是one-to-many设计，即一个闻上账户允许绑定多个微信账户，但一个微信账户不允许绑定超过1个闻上账户；
    + 不考虑微信账户赠与情况
    + 微信并不回收账户Id
3. 闻上账户与手机号码之间事实上是many-to-many关系，当出现闻上账户与手机号码的many to one关系时，该手机号处于冲突（Conflict）状态，需要仲裁机制，其相关功能受限，不得作为MFA依据。
4. 

# 闻上账户

每个用户具有唯一ID；

每个用户必须设置符合格式要求的密码；

每个用户需要提供一种找回

# 登录

用户可以使用密码登录；

用户可以使用微信登录；

用户可以使用手机验证码登录；

# 找回密码

# 修改

USER

```
USERID

```