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

