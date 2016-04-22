#! /bin/sh
export PATH=$PATH:/usr/local/bin:/usr/bin:/usr/local/sbin

dir=.
curfiles=$(ls *.jsp)
echo $curfiles
for file in $curfiles 
do 
	if [ -f $file ]
		then 
		#sed 's:a:b:g' $file>$file.gb
		cat $file | iconv -f 'gb2312' -t 'UTF-8' -o $file.gb
		rm -f $file
		mv $file.gb $file
	fi
done

