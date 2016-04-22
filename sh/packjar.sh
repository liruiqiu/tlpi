#!/bin/sh

# description: 打包jar文件的脚本
# /home/script/flag 文件是脚本运行状态标志，0表示脚本没有运行，1表示脚本正在运行。
# /tmp/ebooktmp/srcfile/120KB.jar 是打包成120KB的JAR文件的模板文件
# /tmp/ebooktmp/srcfile/60KB.jar 是打包成60KB的JAR文件的模板文件
# /tmp/ebooktmp/srcfile/text.txt.dist 是生成text文件的原始文件


#设置中文环境变量
LANG="zh_CN.GB18030"
LANGUAGE="zh_CN.GB18030:zh_CN.GB2312.zh_CN:zh_CN.UTF-8"
SYSFONT="latarcyrheb-sun16"


# flag 1　脚本正在运行
#lag 0  没有脚本在运行

flag=$(cat /home/script/flag)

if [ ${flag} -eq 1 ]
then
	echo "----------------------------"
	echo "$0 脚本已在运行: $(date)"
	echo "----------------------------"
	exit 1
fi


var=$(ps axuf|grep "sh /home/script/packjar.sh"|grep -v grep|wc -l)

bindir=/usr/local/mysql-5.0.20/bin
if [ ! -d ${bindir} ]
then
	bindir=/usr/bin
	if [ ! -d ${bindir} ]
	then
		echo "mysql目录未找到!!!"
		exit 1
	fi
fi

bid=$($bindir/mysql -h 192.168.0.39 -uwaptx -p"(YDtx405)" -P12337 --default-character-set=gbk txbook -e "select bookid from ebook_pack where type=0 limit 20"|sed 1d)

if [[ -z ${bid} ]]
then
	echo "现在没有电子书要生成!!!!"
	exit 1
fi

echo "1"  > /home/script/flag

#echo "bookname:$bookName"
#echo "bookid:$bid"
tmpDir="/tmp/ebooktmp"
srcFile="$tmpDir/srcfile"
errLogPath="/home/script/log"

if [ ! -d $errLogPth ]
then
	mkdir -p ${errLogPath}
fi

for bookId in $bid
do	
	tmpDir="/tmp/ebooktmp/$bookId"
	if [ ! -d $tmpDir ]
	then
		mkdir -p $tmpDir
	fi
	rm -rf $tmpDir/*.txt $tmpDir/part*

	bookName=$($bindir/mysql -h 192.168.0.39 -uwaptx -p"(YDtx405)" -P12337 --default-character-set=gbk txbook -e "select bname from chapter where id=${bookId} limit 1"|sed 1d) 2>>${errLogPath}/outerr.log
	bidPath=$($bindir/mysql -h 192.168.0.39 -uwaptx -p"(YDtx405)" -P12337 --default-character-set=gbk txbook -e "select path from chapter where id=${bookId} limit 1"|sed 1d) 2>>${errLogPath}/outerr.log
	bookIdPath=$(dirname ${bidPath})
	if [ $? -eq 1 ]
	then
		echo "bookname变量赋值错误!!!!" >> ${errLogPath}/outerr.log
		exit 1
	fi

	echo "开始打包电子书 $bookName (${bookId}): $(date)"
	echo "---------------------start------------------------"
	echo "bookid:$bookId"
	echo "bookname:$bookName"
	$bindir/mysql -h 192.168.0.39 -uwaptx -p"(YDtx405)" -P12337 --default-character-set=gbk txbook -e "select cid,'|',cname,'|',path,'|',size,'|',len from chapter where id=${bookId}"|sed 1d > $tmpDir/${bookId}.txt 2>>${errLogPath}/outerr.log
	if [ $? -eq 1 ]
	then
		echo "$tmpDir/${bookId}.txt文件生成错误!!!!" >> ${errLogPath}/outerr.log
	fi
#	bname=$($bindir/mysql -h 192.168.0.39 -uwaptx -p"(YDtx405)" -P12337 txbook -e "select bname from chapter where id=${bookId} and type=0 limit 1"|sed 1d) 
	if [ ! -f $tmpDir/${bookId}.txt ]
	then
		echo "$tmpDir/${bookId}.txt文件未生成"  >> ${errLogPath}/outerr.log
		exit 1
	fi
	#$bindir/mysql -h 192.168.0.39 -uwaptx -p"(YDtx405)" -P12337 txbook -e "select cid,'|',cname,'|',path,'|',size,'|',len from chapter where id=${bookId} and type=0"|sed 1d 
#	echo ""

	for partSize in "60KB" "120KB" "500KB"
	do
		echo "将$bookName($bookId)打包成$partSize的JAR文件"
		if [ "${partSize}" == "60KB" ]
		then
			var=55000
			type=0
		fi
		if [ "${partSize}" == "120KB" ]
		then
			var=130500
			type=1
		fi
		if [ "${partSize}" == "500KB" ]
		then
			var=750000
			type=2
		fi
	
		# 统计所切割文件大小的总和，并按60KB和120KB分组
		total=0
		part=1
		num=1
		lineTotal=$(cat $tmpDir/${bookId}.txt|wc -l)
		#echo $lineTotal
		lineNum=0
		while read line 
		do
			cid=$(echo $line |awk -F "|" '{print $1}')
			cname=$(echo $line|awk -F "|" '{print $2}')
			path=$(echo $line|awk -F "|" '{print $3}')
			size=$(echo $line|awk -F "|" '{print $4}')
			len=$(echo $line|awk -F "|" '{print $5}')
			lineNum=$(echo $lineNum+1|bc)
			if [ ${total} -lt ${var} ]
			then
				num=$(echo $num+1|bc)
				total=$(echo "$total+$size" | bc)
				echo "$cid|$cname|$path|$size|$len" >> $tmpDir/tx${bookId}${partSize}${part}.txt
				echo "$cid|$cname|$path|$size|$len >> $tmpDir/tx${bookId}${partSize}${part}.txt"
			else
				echo "$cid|$cname|$path|$size|$len" >> $tmpDir/tx${bookId}${partSize}${part}.txt
				echo "$cid|$cname|$path|$size|$len >> $tmpDir/tx${bookId}${partSize}${part}.txt"
				total=0
				num=0
				if [ ${lineNum} -lt ${lineTotal} ]
				then
					part=$(echo $part+1|bc)
				fi
			fi
		done < $tmpDir/${bookId}.txt
	
		echo "${bookId}根据${partSize}分成的部分:${part}"
		for i in $(seq ${part})
		do
			echo "处理电子书($bookId)第$i部分"
			partDir="$tmpDir/${partSize}/part${i}"
			metaPath="$partDir/META-INF"
			distPath="/download/ebook/pack/$(echo ${bookId}%128|bc)/${bookId}"
			if [ ! -d $metaPath ]
			then
				mkdir -p $metaPath
			fi
			if [ ! -d $distPath ]
			then
				mkdir -p $distPath
			fi
			# 生成text文件
			echo "" >> $partDir/text.txt
			echo "$bookName" >> $partDir/text.txt
			if [ -f $srcFile/text.txt.dist ]
			then
				cat $srcFile/text.txt.dist >>  $partDir/text.txt
			else
				echo "$srcFile/text.txt.dist文件不存在"
				exit 1
			fi
			# 转换text文件为win格式并转换成utf-8文件格式
			if [ -f $partDir/text.txt ]
			then
				sed -e 's/$/\r/' < $partDir/text.txt >  $partDir/text.1
			else
				echo "$partDir/text.txt 不存在"
				exit 1
			fi
			if [ -f $partDir/text.1 ]
			then
				iconv  -t UTF-8 $partDir/text.1 > $partDir/text 2>>${errLogPath}/outerr.log
				if [ $? -eq 1 ]
				then
					echo "转换$partDir/text.1为UTF-8编码出错!!!!" >> ${errLogPath}/outerr.log
				fi
			else
				echo "$partDir/text.1 不存在"
				exit 1
			fi

			rm -rf $partDir/text.txt $partDir/text.1
	
			# 生成MF文件
			echo "Manifest-Version: 1.0" >> $metaPath/MANIFEST
			echo "MicroEdition-Configuration: CLDC-1.0" >> $metaPath/MANIFEST
			echo "MIDlet-Name: tx${bookId}${partSize}${i}" >> $metaPath/MANIFEST
			echo "MIDlet-Icon: /icon.png" >> $metaPath/MANIFEST
			echo "Created-By: 1.5.0-beta (Sun Microsystems Inc.)" >> $metaPath/MANIFEST
			echo "MIDlet-Vendor: www.tx.com.cn" >> $metaPath/MANIFEST
			echo "MIDlet-1: tx${bookId}${partSize}${i},/icon.png,demo" >> $metaPath/MANIFEST
			echo "MIDlet-Version: 1.0.0" >> $metaPath/MANIFEST
			echo "MicroEdition-Profile: MIDP-1.0" >> $metaPath/MANIFEST

			# 转换成win格式并转换成utf-8格式
			sed -e 's/$/\r/' <  $metaPath/MANIFEST >  $metaPath/MANIFEST.1
			iconv  -t UTF-8 $metaPath/MANIFEST.1 >$metaPath/MANIFEST.MF 2>>${errLogPath}/outerr.log
			if [ $? -eq 1 ]
			then
				echo "转换$metaPath/MANIFEST.1为UTF-8编码出错!!!!" >> ${errLogPath}/outerr.log
			fi

			rm -rf $metaPath/MANIFEST $metaPath/MANIFEST.1
	
			lenTotal=0   # 统计每个部分的文字总个数变量
			flag=0
			num=0  # 每个PART中的文件个数变量
			var=1   # 所切割文件名第1位数字变量
			echo "" >> ${partDir}/capter.txt   # capter.txt第一行为空
			echo "http://221.130.176.136/books/readBookInfo.do?bookId=${bookId}" >> ${partDir}/capter.txt

			# 第一个章节ID
			cidfirst=$(cat $tmpDir/tx${bookId}${partSize}${i}.txt|sed -n 1p |awk -F "|" '{print $1}') 

			# 第一个章节名
			cnamefirst=$(cat $tmpDir/tx${bookId}${partSize}${i}.txt|sed -n 1p |awk -F "|" '{print $2}')

			#echo "cidfirst: $cidfirst | cnamefirst: $cnamefirst"
			numvar=0  # 文件名后一位变量

			if [ -f $distPath/tx${bookId}${partSize}${i}.txt ]
			then
				rm -rf $distPath/tx${bookId}${partSize}${i}.txt
			fi
			
	
			# 增加UTF-8头标志到txt文件 
			if [ "${partSize}" == "120KB" ]
			then
				echo "生成 $distPath/tx${bookId}${partSize}${i}.txt 文件"
				printf "\xEF\xBB\xBF" > $distPath/tx${bookId}${partSize}${i}.txt
			fi	

			while read line 
			do
				cid=$(echo $line|awk -F "|" '{print $1}')
				cname=$(echo $line|awk -F "|" '{print $2}')
				path=$(echo $line|awk -F "|" '{print $3}')
				size=$(echo $line|awk -F "|" '{print $4}')
				len=$(echo $line|awk -F "|" '{print $5}')
				if [ "${partSize}" == "120KB" ]
				then
					cat ${path} | sed 1d >> $distPath/tx${bookId}${partSize}${i}.txt
					#cat ${path} |sed 1d 
				fi
				# cp -f --reply=yes $path $partDir 
				if [ ${cid} -eq ${cidfirst} ]
				then
					#numvar=$(echo ${numvar}|wc -c)
					if [ ${numvar} -lt 10 ]
					then
						numvar="0${numvar}"
					fi
			#		echo $num
					if [ ${i} -eq 1 ]
					then
						cp -f --reply=yes $path $partDir/ 2>>${errLogPath}/outerr.log
						if [ $? -eq 1 ]
						then
							echo "cp $path到$partDir目录出错!!!!!" >> ${errLogPath}/outerr.log
							exit 1
						fi
						echo "cid:${cid}==cidfirst:${cidfirst}&&part${i}==1>>>>cp -f --reply=yes $path $partDir/"
					else
						cp -f --reply=yes $path $partDir/${var}00${numvar} 2>>${errLogPath}/outerr.log
						if [ $? -eq 1 ]
						then
							echo "cp $path到$partDir/${var}00${numvar}目录出错!!!!!" >> ${errLogPath}/outerr.log
							exit 1
						fi
						echo "cid:${cid}==cidfirst:${cidfirst}&&part${i}!=1>>>cp -f --reply=yes $path $partDir/${var}00${numvar}"
					fi
					
					lenTotal=$(echo "$lenTotal+$len"|bc)
					num=$(echo ${num}+1|bc)
					numvar=$(echo ${numvar}+1|bc)
					#echo $num
					cidfirst=${cid}
					cnamelast=${cname}
				else
					echo "$cnamelast $num $total"
					if [ ${num} -lt 10 ]
					then
						num="0${num}"
					fi
					var=$(echo ${var}+1|bc)
					numvar=0
					if [ ${numvar} -lt 10 ]
					then
						numvar="0${numvar}"
					fi
					if [ ${i} -eq 1 ]
					then
						cp -f --reply=yes $path $partDir/ 2>>${errLogPath}/outerr.log
						if [ $? -eq 1 ]
						then
							echo "cp $path到$partDir目录出错!!!!!" >> ${errLogPath}/outerr.log
							exit 1
						fi
						echo "cid:${cid}!=cidfirst:${cidfirst}&&part${i}==1>>>cp -f --reply=yes $path $partDir/"
					else
						cp -f --reply=yes $path $partDir/${var}00${numvar} 2>>${errLogPath}/outerr.log
						if [ $? -eq 1 ]
						then
							echo "cp $path到$partDir/${var}00${numvar}目录出错!!!!!" >> ${errLogPath}/outerr.log
							exit 1
						fi
						echo "cid:${cid}!=cidfirst:${cidfirst}&&part${i}!=1>>>cp -f --reply=yes $path $partDir/${var}00${numvar}"
					fi
					echo "${cnamelast}${num}" >> ${partDir}/capter.txt
					echo "$lenTotal" >> ${partDir}/capter.txt
					echo "bookid:${bookId}>>bookname:${bookName}>>cname:$cnamelast>>num of the file:$num>>num of the word:$lenTotal"
					cidfirst=${cid}
					lenTotal=${len}
					numvar=$(echo ${numvar}+1|bc)
					num=1
				fi
			done < $tmpDir/tx${bookId}${partSize}${i}.txt
			if [ ${numvar} -lt 10 ]
			then
				numvar="0${numvar}"
			fi
			#echo "numvar:$numvar"
			if [ ${num} -lt 10 ]
			then
				num="0${num}"
			fi
			echo "bookid:${bookId}>>bookname:${bookName}>>$cnamelast>>num of the file:$num>>num of the word:$lenTotal"
			echo "${cnamelast}${num}" >> ${partDir}/capter.txt
			echo "$lenTotal" >> ${partDir}/capter.txt

			# 转换capter文件为win格式
			sed -e 's/$/\r/' <   ${partDir}/capter.txt  >   ${partDir}/capter.1
			iconv  -t UTF-8 ${partDir}/capter.1 > ${partDir}/capter 2>>${errLogPath}/outerr.log
			if [ $? -eq 1 ]
                        then
                                echo "转换$metaPath/capter.1为UTF-8编码出错!!!!" >> ${errLogPath}/outerr.log
                        fi
			rm -rf ${partDir}/capter.txt ${partDir}/capter.1
		
			if [ ! -f $srcFile/${partSize}.jar ]
			then
				echo "$srcFile/${partSize}.jar 不存在"
				exit 1
			fi
			cp --reply=yes -f $srcFile/${partSize}.jar $distPath/tx${bookId}${partSize}${i}.jar 2>>${errLogPath}/outerr.log
			if [ $? -eq 1 ]
			then
				echo "cp $srcFile/${partSize}.jar>>$distPath/tx${bookId}${partSize}${i}.jar出错!!!!"  >> ${errLogPath}/outerr.log
			fi
			echo "cp --reply=yes -f $srcFile/${partSize}.jar $distPath/tx${bookId}${partSize}${i}.jar"
			/usr/local/jdk1.5.0_06/bin/jar uvfM $distPath/tx${bookId}${partSize}${i}.jar -C $partDir /

			# 生成JAD文件
			echo "生成JAD文件: $distPath/tx${bookId}${partSize}${i}.jad"

			if [ -f $distPath/tx${bookId}${partSize}${i}.jad ]
			then
				rm -rf $distPath/tx${bookId}${partSize}${i}.jad
			fi
			if [ -f $distPath/tx${bookId}${partSize}${i}.jar ]
			then
				jarSize=$(ls -l $distPath/tx${bookId}${partSize}${i}.jar|awk '{print $5}' )
				jarFileName=$(echo $distPath/tx${bookId}${partSize}${i}.jar|awk -F/ '{print $NF}'|awk -F. '{print $1}')
				echo -e "Manifest-Version: 1.0\r" >> $distPath/tx${bookId}${partSize}${i}.jad
				echo -e "MicroEdition-Configuration: CLDC-1.0\r" >> $distPath/tx${bookId}${partSize}${i}.jad
				echo -e "MIDlet-Name: $jarFileName\r" >> $distPath/tx${bookId}${partSize}${i}.jad
				echo -e "MIDlet-Icon: /icon.png\r" >> $distPath/tx${bookId}${partSize}${i}.jad
				echo -e "Created-By: 1.5.0-beta (Sun Microsystems Inc.)\r" >> $distPath/tx${bookId}${partSize}${i}.jad
				echo -e "MIDlet-Vendor: www.tx.com.cn\r" >> $distPath/tx${bookId}${partSize}${i}.jad
				echo -e "MIDlet-1: text,/icon.png,demo\r" >>$distPath/tx${bookId}${partSize}${i}.jad
				echo -e "MIDlet-Version: 1.0.0\r" >>$distPath/tx${bookId}${partSize}${i}.jad
				echo -e "MicroEdition-Profile: MIDP-1.0\r" >>$distPath/tx${bookId}${partSize}${i}.jad
				echo -e "MIDlet-Jar-Size: $jarSize\r" >>$distPath/tx${bookId}${partSize}${i}.jad
				echo -e "MIDlet-Jar-URL:${jarFileName}.jar\r" >>$distPath/tx${bookId}${partSize}${i}.jad
			else
				echo "$distPath/tx${bookId}${partSize}${i}.jar 文件不存在"  >> ${errLogPath}/outerr.log
				exit 1
			fi

			$bindir/mysql -h 192.168.0.39 -uwaptx -p"(YDtx405)" -P12337 --default-character-set=gbk txbook -e "insert into ebook_info (bookid,path,type) values ('$bookId','tx${bookId}${partSize}${i}','$type')" 2>>${errLogPath}/outerr.log
			if [ $? -eq 1 ]
			then
				echo "insert into ebook_info (bookid,path,type) values ('$bookId','tx${bookId}${partSize}${i}','$type')出错!!!!"  >> ${errLogPath}/outerr.log
			fi
		done
	done
	echo "${bookId} 电子书生成结束: $(date)" 
	echo "删除临时目录：$tmpDir"
	rm -rf $tmpDir
	echo "insert into ebook (bookid,name) values ('$bookId','$bookName')"
	$bindir/mysql -h 192.168.0.39 -uwaptx -p"(YDtx405)" -P12337 --default-character-set=gbk txbook -e "insert into ebook (bookid,name) values ('$bookId','$bookName')" 2>>${errLogPath}/outerr.log
	if [ $? -eq 1 ]
	then
		echo "insert into ebook (bookid,name) values ('$bookId','$bookName') 出错!!!!"  >> ${errLogPath}/outerr.log
	fi
#	$bindir/mysql -h 192.168.0.39 -uwaptx -p"(YDtx405)" -P12337 txbook -e "update chapter set type=1 where id=${bookId}"
	echo "update ebook_pack set type=1 where bookid=${bookId}"
	$bindir/mysql -h 192.168.0.39 -uwaptx -p"(YDtx405)" -P12337 --default-character-set=gbk txbook -e "update ebook_pack set type=1 where bookid=${bookId}" 2>>${errLogPath}/outerr.log
	if [ $? -eq 1 ]
	then
		echo "update ebook_pack set type=1 where bookid=${bookId} 出错!!!!"  >> ${errLogPath}/outerr.log
	fi
	echo "删除已压缩的电子书原文件: ${bookIdPath}"
	rm -rf ${bookIdPath}
	echo "----------------END-------------------"
done
echo "更改标志"
echo "0"  > /home/script/flag
