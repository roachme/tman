#!/usr/bin/env bash

function tman()
{
    PATHTAG="PWD: "
    output="$(_tman "$@")"
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

# roach: Under development
_tman_comp()
{
    if [ "$3" = "foo" ]; then
        COMPREPLY=($(compgen -d "heh"))
    else
        COMPREPLY=($(compgen -W "foo bar baz" -- "$2"))
    fi
}

#complete -F _tman_comp tman
complete -W "cfg init pgm add del prev set sync use prj move show help list ver" tman

