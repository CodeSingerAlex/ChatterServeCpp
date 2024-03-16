#!/usr/bin/env sh

# abort on errors
set -e

git checkout -b feature-group-chat
git add -A
git commit -m '新增群聊功能'

git push -f git@github.com:CodeSingerAlex/ChatterServeCpp.git feature-group-chat