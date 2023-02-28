# 遍历_Driver_Object

关于遍历 内核模块 ，网上有许多代码，一般都是通过遍历  PDriver_Object->DriverSection（_LDR_DATA_TABLE_ENTRY）实现，遍历 _Driver_Object 的好像没找到。

#### 这个方法的原理就是：

内核对象保存着它所在目录的对象指针。（这句话好像有点难理解）

我们遍历这个目录指针内的所有对象，就是我们要的 PDriver_Object ，这个目录就是 WinObj 里的 Driver 目录（下面有图） 



先介绍 `_OBJECT_HEADER` ：这个结构是每个内核对象都有的。

###### _OBJECT_HEADER

```c
3: kd> dt _Object_header
nt!_OBJECT_HEADER
   +0x000 PointerCount     : Int8B //Object引用次数
   +0x008 HandleCount      : Int8B //Handle引用次数
   +0x008 NextToFree       : Ptr64 Void
   +0x010 Lock             : _EX_PUSH_LOCK
   +0x018 TypeIndex        : UChar //需要这个获取Object类型
   +0x019 TraceFlags       : UChar
   +0x019 DbgRefTrace      : Pos 0, 1 Bit
   +0x019 DbgTracePermanent : Pos 1, 1 Bit
   +0x01a InfoMask         : UChar
   +0x01b Flags            : UChar
   +0x01b NewObject        : Pos 0, 1 Bit
   +0x01b KernelObject     : Pos 1, 1 Bit
   +0x01b KernelOnlyAccess : Pos 2, 1 Bit
   +0x01b ExclusiveObject  : Pos 3, 1 Bit
   +0x01b PermanentObject  : Pos 4, 1 Bit
   +0x01b DefaultSecurityQuota : Pos 5, 1 Bit
   +0x01b SingleHandleEntry : Pos 6, 1 Bit
   +0x01b DeletedInline    : Pos 7, 1 Bit
   +0x01c Reserved         : Uint4B
   +0x020 ObjectCreateInfo : Ptr64 _OBJECT_CREATE_INFORMATION
   +0x020 QuotaBlockCharged : Ptr64 Void
   +0x028 SecurityDescriptor : Ptr64 Void
   +0x030 Body             : _QUAD  //这里往下就是我们用到的内核对象
```

看到`+0x030 Body` 这一句没，从这里开始就是我们真正用到的内核对象，例：

我们手上有一个 PDriver_Object ，那么PDriver_Object - 0x30 就是 PObject_header



再介绍 `_OBJECT_HEADER_NAME_INFO`

```c
3: kd> dt _OBJECT_HEADER_NAME_INFO
nt!_OBJECT_HEADER_NAME_INFO  //size 0x20
   +0x000 Directory        : Ptr64 _OBJECT_DIRECTORY //对象所在目录的对象指针
   +0x008 Name             : _UNICODE_STRING //ObjectName 
   +0x018 ReferenceCount   : Int4B
   +0x01c Reserved         : Uint4B
```

一个内核对象的POBJECT_HEADER_NAME_INFO = PObject_header - sizeof(_OBJECT_HEADER_NAME_INFO)

 

###### 小总结：

_OBJECT_HEADER_NAME_INFO 在 _Object_header 头上 ， _Driver_Object 在 _Object_header 里面。



`_OBJECT_HEADER_NAME_INFO.Directory`就是我上面说的 “对象所在目录的对象指针”（Driver目录）



###### _Object_directory

```c
2: kd> dt ffffc40f`a6e07c90 _Object_directory
nt!_OBJECT_DIRECTORY
   +0x000 HashBuckets      : [37] (null) //37个指针空间 POBJECT_DIRECTORY_ENTRY
   +0x128 Lock             : _EX_PUSH_LOCK
   +0x130 DeviceMap        : (null) 
   +0x138 ShadowDirectory  : (null) 
   +0x140 NamespaceEntry   : (null) 
   +0x148 SessionObject    : (null) 
   +0x150 Flags            : 0
   +0x154 SessionId        : 0xffffffff
```

37个指针空间内存放着  POBJECT_DIRECTORY_ENTRY 。



######  _OBJECT_DIRECTORY_ENTRY

```c
2: kd> dt 0xffffc40faa0c5520 _OBJECT_DIRECTORY_ENTRY //上面第二个
nt!_OBJECT_DIRECTORY_ENTRY
   +0x000 ChainLink        : 0xffffc40f`a6e284b0 _OBJECT_DIRECTORY_ENTRY //因为37个指针空间装不完所有对象
   +0x008 Object           : 0xffff9d0e`5688ce50 Void //这个就是 PDriver_Object
   +0x010 HashValue        : 0xb8888f8d
```

37个指针都是指向这样一个结构，里面的 Object 就是 我们要的 PDriver_Object。

因为37个指针是装不完我们全部的 PDriver_Object，所以 _OBJECT_DIRECTORY_ENTRY 里面的 ChainLink 也是指向一个 新的 _OBJECT_DIRECTORY_ENTRY ，新的 _OBJECT_DIRECTORY_ENTRY  里的Object又是一个 PDriver_Object。



这样我们遍历 37 个 POBJECT_DIRECTORY_ENTRY  和里面的 ChainLink ，就能获得全部的 PDriver_Object。





