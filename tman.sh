#!/usr/bin/env bash

function tman()
{
    local pwdfile="/tmp/tmanpwd"
    _tmancli "$@" || return $? && test -s "$pwdfile" && cd "$(cat "$pwdfile")" || return 1
}
