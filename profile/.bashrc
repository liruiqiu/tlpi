# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi
alias grep='grep --color=auto -n'
HISTSIZE=100000000

# Uncomment the following line if you don't like systemctl's auto-paging feature:
# export SYSTEMD_PAGER=

# User specific aliases and functions

function mkf()
{
make clean-framework -j8 && \
mmm frameworks/base/ -j8 #&& \
#make -j8 systemimage
#adb remount && \
#adb push $OUT/system/framework/framework.jar /system/framework/ && \
#adb reboot
}


function mkp()
{
make clean-android.policy -j8 && \
mmm frameworks/base/policy -j8 #&& \
#make -j8 systemimage
#adb remount && \
#adb push $OUT/system/framework/android.policy.jar /system/framework/ && \
#adb reboot
#adb kill-server && adb start-server
}


function mkr()
{
make clean-framework-res -j8 && \
mmm frameworks/base/core/res/ -j8 #&& \
#make -j8 systemimage
#adb remount && \
#adb push $OUT/system/framework/framework-res.apk /system/framework/ && \
#adb reboot
}
function mks()
{
mmm packages/apps/Settings/ -j8
}

function mksys()
{
make -j8 systemimage
}
function mkboot()
{
make -j8 bootimage
}

function mkall()
{
	mkf
		mkr
		mkp
		mks
		mksys
}

function flashboot()
{
adb reboot bootloader && sleep 3&& fastboot flash boot  out/target/product/maguro/boot.img && fastboot reboot
}

function flashall()
{
adb reboot bootloader && sleep 3&& fastboot flashall -w
}

echo "load ~/.bashrc"
