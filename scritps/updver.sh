#!/usr/bin/env bash

set -e

NEWVERSION="$1"

if [ -z "$NEWVERSION" ]; then
    echo "Usage: updver.sh NEWVERSION"
    exit 1
fi


#TODO: check NEWVERSION value. Pattern: v<0-255>.<0-255>.<0-255>.
#TODO: check NEWVERSION is greater than previous and does not exist already

git checkout master

sed -i "/#define VERSION/s/\(#define VERSION\s*\).*/\1\"$NEWVERSION\"/" lib/common.h
git add lib/common.h
git commit -m "tman: Bump version to $NEWVERSION"

git tag "$NEWVERSION"
git tag --delete latest
git tag latest "$NEWVERSION"

git push origin "$NEWVERSION"
git push origin latest --force # overwrite previous latest tag

git checkout develop
git rebase master
