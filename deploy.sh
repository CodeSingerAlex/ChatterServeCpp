#!/usr/bin/env sh

# abort on errors
set -e

git checkout -b feature-redis
git add -A
git commit -m '添加服务器中间件'

git push -f git@github.com:CodeSingerAlex/ChatterServeCpp.git feature-redis