#!/usr/bin/env sh

# abort on errors
set -e

git add -A
git commit -m '新增一对一聊天功能'

git push -f git@github.com:CodeSingerAlex/ChatterServeCpp.git main