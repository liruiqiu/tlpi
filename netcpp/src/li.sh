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
#       CREATED: 2014��12��02�� 10ʱ24��32�� CST
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
find . -name "*.h" -o -name "*.c" -o -name "*.cc" -o -name "*.cpp" > cscope.files && cscope -bkq -i cscope.files
