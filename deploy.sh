#!/usr/bin/env sh

# abort on errors
set -e

git checkout -b feature-ChatClient
git add -A
git commit -m '添加聊天客户端'

git push -f git@github.com:CodeSingerAlex/ChatterServeCpp.git feature-ChatClient