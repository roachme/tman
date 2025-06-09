#!/usr/bin/env bash

function tman()
{
    local tmanstatus;
    local pwdfile="/tmp/tmanpwd"

    _tmancli "$@"
    tmanstatus="$?"

    test -s "$pwdfile" && cd "$(cat "$pwdfile")" || return "$tmanstatus"
    return "$tmanstatus"
}
