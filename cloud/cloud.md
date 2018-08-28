# Services

1. tribe
2. update (nas firmware/service, android, iOS app store, electron...)

This document focuses on tribe only.

# Deployment

## static

LB -> Services (1..n) -> database

1. LB has a unique domain name, https
2. Each service has a unique domain name (二级域名，自签)


```
Example:

https://www.wstribe.com
https://service-0001.wstribe.com
```
## dynamic

CD

# Resources & Features

## user account

```
user {
    uuid: globally unique user id in wisnuc tribe domain,
    password:
    passwordModifiedTime:

    status: PENDING, ACTIVE, DISABLED,
    
    createdAt: ???,	
    updateAt:

    email: // good to have

    thirdPartyAccoutBinding: []，
    credentialRevocation: [],
    phoneNumbers: []
}

user - wechat binding {
    unionId: // identifier
    nickName:
    avatarUrl:
    bindingTime: ???
    lastModified: ??? // nickname & avatarUrl update time
}

```

### activation

sms verification
Email verification

### strong authentication and weak authentication

weak: login (password, third-party)

strong: change login authentication credentials. including 
1. password
2. third-party

- 2-factor authentication
- password must not be the second factor, then what is it?


+ phone number as the 2nd factor
    + user is allowed to add / delete phone number
+ phone number or email as the 2nd factor
    + changing phone number requires email authentication

trust 1. secret & chaining 2. web of trust PGP

### hand-over

### password revocation

phone or email, depend on user settings (fixed)

## Station Account

aws-iot api, hardware crypto certificate;

based on aws iot framework.


## Arrows / Bi-directional User Station Relationship

Left Arrow (station service global user): on / off
Right Arrow (user agreed to consume station service): tri-state: undefined, yes, no

## Message Box


User, Login

1. user information
2. my devices
    - I owned
    - who serves me? (station list)
3. messages



# Interface (API)

# Design

