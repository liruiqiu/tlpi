JAVA_HOME=/usr/java/jdk1.6.0_45
CLASSPATH=.:$JAVA_HOME/lib/tools.jar:$JAVA_HOME/lib/dt.jar
ANDROID_HOME=$(pwd)
ANDROID_PRODUCT_OUT=$ANDROID_HOME/out/target/product/maguro
PATH=$ANDROID_HOME/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin:$JAVA_HOME/bin:$ANDROID_HOME/out/host/linux-x86/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/tools:$ANDROID_HOME/ndk:$PATH
export JAVA_HOME CLASSPATH ANDROID_PRODUCT_OUT PATH
export TARGET_PREBUILT_KERNEL=../kernel/omap/arch/arm/boot/zImage


source build/envsetup.sh
lunch full_maguro-userdebug


#android kernel
ANDROID_HOME=../../android-4.3_r1.1
PATH=$ANDROID_HOME/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin:$JAVA_HOME/bin:$ANDROID_HOME/out/host/linux-x86/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/tools:$PATH

export ARCH=arm
export SUBARCH=arm
export CROSS_COMPILE=arm-eabi-

make  tuna_defconfig

