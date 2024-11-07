#!/usr/bin/env sh

function tman()
{
    CWD="$(pwd)"

    cd /home/roach/.workspace/sewer/sketch/c
    output="$(./tman "$@")"
    retcode="$?"
    if [ "$retcode" -eq 0 ]; then
        cd "$output"
    else
        [ -n "$output" ] && echo "$output"
    fi
}
