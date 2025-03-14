#!/usr/bin/env bash

#TODO: check NEWVERSION value. Pattern: v<0-255>.<0-255>.<0-255>.
#TODO: check NEWVERSION is greater than previous and does not exist already
#TODO: update CHANGELOG: set new version and date


VERSION="$1"
# TODO: regex does not check number limits 0 < VERSION <= 255
VERSION_PATTERN="v[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}"

function usage()
{
    echo "Usage: updver.sh VERSION"
    echo "VERSION pattern v[0-255].[0-255].[0-255]"
    return 0
}

function die()
{
    usage
    exit 1
}

function update_master()
{
    if ! git checkout --quiet master; then
        echo "Current branch has uncommited changes. Cannot switch to master"
        exit 1
    fi

    sed -i "/#define VERSION/s/\(#define VERSION\s*\).*/\1\"$NEWVERSION\"/" lib/common.h
    git add lib/common.h
    git commit -m "tman: Bump version to $NEWVERSION"

    echo
    echo "Update: CHANGELOG.md"
}

function update_develop()
{
    return 0
}

function update_remote()
{
    return 0
}

if [ -z "$VERSION" ]; then
    echo "updver: Version is missing"
    die
elif ! echo "$VERSION" | grep -wq "$VERSION_PATTERN"; then
    echo "updver: Invalid version"
    die
fi

update_master
