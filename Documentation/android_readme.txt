

编译源码并刷到Galaxy Nexus I9250真机上

版本：
pc 	CentOS 7.0  x86_64
手机    Galaxy Nexus I9250
系统    android-4.3_r1.1
内核	android-omap-tuna-3.0-jb-mr2


目录结构 
 android---	
		|
        	|	android-4.3_r1.1      安卓系统源码
		|	kernel/omap           安卓系统源码对应的内核
		|	rpm	                  安卓系统源码编译依赖的软件
		|	takju-jwr66y     	  Galaxy Nexus I9250 官方系统 
		|	takju-jwr66y/device   Galaxy Nexus I9250 官方 android-4.3_r1.1 需要的驱动
		|	70-android.rules      可信的设备文件 
		|	参考                  	参考的网上资料
		|	说明.txt              说明文档
			
依赖软件
cd android/rpm
sudo rpm -ivh jdk-6u45-linux-amd64.rpm

添加可信的设备文件
sudo cp android/70-android.rules /etc/udev/rules.d/70-android.rules  


编译内核
cd android/kernel/omap
source env.sh      #载入环境变量

编译源码
cd android/android-4.3_r1.1
source env.sh      #载入环境变量
make -j8		#8线程编译 

刷机：
adb reboot bootloader # 重启进入bootloader
fastboot flashall -w  # 刷机


实现国密（sm4）磁盘加密修改如下
安卓系统源码
system/vold/cryptfs.c:1364
-    strcpy((char *)crypt_ftr.crypto_type_name, "aes-cbc-essiv:sha256");
替换
+    strcpy((char *)crypt_ftr.crypto_type_name, "sm4-cbc-essiv:sha256"); 

安卓系统源码对应的内核  
增加SM4模块代码
include/crypto/sm4.h
crypto/sm4_generic.c

将SM4模块添加到编译文件中
[lrq@localhost omap]$ git diff crypto/Makefile
diff --git a/crypto/Makefile b/crypto/Makefile
index ce5a813..fa4243a 100644
--- a/crypto/Makefile
+++ b/crypto/Makefile
@@ -65,6 +65,7 @@ obj-$(CONFIG_CRYPTO_TWOFISH) += twofish_generic.o
 obj-$(CONFIG_CRYPTO_TWOFISH_COMMON) += twofish_common.o
 obj-$(CONFIG_CRYPTO_SERPENT) += serpent.o
 obj-$(CONFIG_CRYPTO_AES) += aes_generic.o
+obj-$(CONFIG_CRYPTO_SM4) += sm4_generic.o
 obj-$(CONFIG_CRYPTO_CAMELLIA) += camellia.o
 obj-$(CONFIG_CRYPTO_CAST5) += cast5.o
 obj-$(CONFIG_CRYPTO_CAST6) += cast6.o

默认开启SM4模块
[lrq@localhost omap]$ git diff crypto/Kconfig
diff --git a/crypto/Kconfig b/crypto/Kconfig
index 87b22ca..91fe8fe 100644
--- a/crypto/Kconfig
+++ b/crypto/Kconfig
@@ -562,6 +562,24 @@ config CRYPTO_AES_NI_INTEL
          ECB, CBC, LRW, PCBC, XTS. The 64 bit version has additional
          acceleration for CTR.
 
+config CRYPTO_SM4
+       tristate "SM4 cipher algorithms"
+       default y
+       select CRYPTO_ALGAPI
+       help
+
+         Rijndael appears to be consistently a very good performer in
+         both hardware and software across a wide range of computing
+         environments regardless of its use in feedback or non-feedback
+         modes. Its key setup time is excellent, and its key agility is
+         good. Rijndael's very low memory requirements make it very well
+         suited for restricted-space environments, in which it also
+         demonstrates excellent performance. Rijndael's operations are
+         among the easiest to defend against power and timing attacks.
+
+         The SM4 specifies three key sizes: 128 bits
+

测试

未加密                                                                                
4K 1024次       0m11.94s real     0m0.12s user     0m0.73s system                                     
1M 1024次      1m21.87s real     0m3.01s user     0m13.44s system                                                            
1G 1次         2m28.32s real     0m0.04s user     0m11.10s system                     

AES
0m12.81s real     0m0.07s user     0m0.64s system
2m32.75s real     0m3.34s user     0m13.42s system
4m12.81s real     0m0.03s user     0m9.74s system

SM4
0m12.71s real     0m0.10s user     0m0.59s system
2m51.03s real     0m2.71s user     0m13.41s system
4m23.88s real     0m0.04s user     0m9.85s system



