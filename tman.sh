#!/usr/bin/env sh

function tman()
{
    PATHTAG="PWD: "
    output="$(_tman "$@")"
    retcode="$?"

    # TODO: simplify this shit
    # Check that output contains tag. If so then interpret
    # it as a path not output.
    echo "$output" | grep -qE "^$PATHTAG"
    tagcheck="$?"
    if [ "$tagcheck" -eq 0 ]; then
        cd "${output/$PATHTAG/}"
    elif [ -n "$output" ]; then
        echo "$output"
    fi
    return "$retcode"
}

# roach: Under development
_tman_comp()
{
    if [ "$3" == "foo" ]; then
        COMPREPLY=($(compgen -d "heh"))
    else
        COMPREPLY=($(compgen -W "foo bar baz" -- "$2"))
    fi
}

#complete -F _tman_comp tman
complete -W "cfg init pgm add del prev set sync use env move cat help list ver" tman

