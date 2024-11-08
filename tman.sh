#!/usr/bin/env sh

function tman()
{
    CWD="$(pwd)"

    output="$(_tman "$@")"
    retcode="$?"
    if [ "$retcode" -eq 0 ]; then
        cd "$output"
    else
        [ -n "$output" ] && echo "$output" && cd "$CWD"
    fi
}
