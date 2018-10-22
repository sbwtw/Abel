ecc

芯片的序列号没什么用，版本仅仅用户芯片类型检查（508 or 608）；

preset

writeAwsConfig

写入config是多步骤操作，如果失败结果可能导致地址变化，所以写入地址的操作应该最后进行。

lock config zone

genkey device key，其他的key在做什么研究一下

generate csr (including a sign)

sign

verify

save to slot 8

read from slot 8

lock slot

slot 15 can be used to save a public key 

x509

Other functions are not used.
