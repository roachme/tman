#!/usr/bin/env bash

function tec()
{
    local tecstatus;
    local pwdfile="/tmp/tecpwd"

    teccli "$@"
    tecstatus="$?"

    test -s "$pwdfile" && cd "$(cat "$pwdfile")" || return "$tecstatus"
    return "$tecstatus"
}
