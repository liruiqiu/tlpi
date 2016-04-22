echo $*
echo ${*:2}
: ${1?"usage $0 ARGUMENT"}
echo "****"
echo ${@:3:1}
stringa="abcdef"
echo `expr substr $stringa 2 2`
