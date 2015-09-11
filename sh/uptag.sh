#!/bin/bash - 
#===============================================================================
#
#          FILE:  li.sh
# 
#         USAGE:  ./li.sh 
# 
#   DESCRIPTION:  
# 
#       OPTIONS:  ---
#  REQUIREMENTS:  ---
#          BUGS:  ---
#         NOTES:  ---
#        AUTHOR: YOUR NAME (), 
#       COMPANY: 
#       CREATED: 2015��02��08�� 22ʱ22��05�� CST
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
find . -name "*.h" -o -name "*.c" -o -name "*.cc" > cscope.files
cscope -bkq -i cscope.files

