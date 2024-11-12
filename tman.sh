#!/usr/bin/env sh

function tman()
{
    output="$(_tman "$@")"
    retstatus="$?"
    if [ "$retstatus" -eq 0 ]; then
        cd "$output"
    else
        [ -n "$output" ] && echo "$output"
    fi

    # Status 0 - ok, no error, no need to explain anything.
    # Status 1 - ok, but output is not a path, so juts output it.
    [ "$retstatus" -eq 1 ] && return 0 || return "$retstatus"
    return "$retstatus"
}
