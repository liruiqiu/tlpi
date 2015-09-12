#! /bin/sh
git clone https://github.com/amix/vimrc.git ~/.vim_runtime
sh ~/.vim_runtime/install_awesome_vimrc.sh
cd ~/.vim_runtime
git pull --rebase
git clone https://github.com/purcell/emacs.d.git ~/.emacs.d
