#!/usr/bin/env bash

function tman()
{
    PATHTAG="PWD: "
    output="$(_tmancli "$@")"
    retcode="$?"

    # Check that output contains tag. If so then interpret
    # it as a path not an output.
    if [ -n "$(echo "$output" | grep -E "^$PATHTAG")" ]; then
        cd "${output/$PATHTAG/}" || return 1
    elif [ -n "$output" ]; then
        echo "$output"
    fi
    return "$retcode"
}
