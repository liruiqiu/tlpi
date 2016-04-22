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
#       CREATED: 2014年12月02日 10时24分32秒 CST
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
find . -name "*.h" -o -name "*.c" -o -name "*.cc" -o -name "*.cpp" > cscope.files && cscope -bkq -i cscope.files
