#!/bin/bash

TMAN=""
TMAN_CWD=

# config vars
TMAN_PREFIX=
TMAN_INSTALL=

TMAN_TMANCONF=


# tman -b basepath -c configpath CMD OPTIONS arg


function _tman_wd_add()
{
    local wpoint="$1"
    command -v wd &>/dev/null
    local ret="$?"
    if [ "$ret" -eq 0 ]; then
        wd add -q -f "$wpoint"
    fi
}

function _tman_wd_rm()
{
    local wpoint="$1"
    command -v wd &>/dev/null
    local ret="$?"
    if [ "$ret" -eq 0 ]; then
        wd rm -q "$wpoint"
    fi
}


function _tman_get_tmanconf()
{
    TMAN_TMANCONF="${HOME}/.config/tman/sys.conf"
}

function _tman_get_sys_config_vars()
{
    _tman_get_tmanconf
    TMAN_PREFIX="$(grep prefix "$TMAN_TMANCONF" | cut -f 2 -d '=' | tr -d ' ' | tr -d '"' | tr -d "'")"
    TMAN_INSTALL="$(grep install "$TMAN_TMANCONF" | cut -f 2 -d '=' | tr -d ' ' | tr -d '"' | tr -d "'")"
}

function _tman_handle_command()
{
    local cmd="$1"
    local taskid=
    local taskdir=

    # get env from the file
    TMAN_CURR="$(cat "${TMAN_PREFIX}/.tman/curr" 2>/dev/null)"

    if [ "$cmd" = "add" ]; then
        local taskdir="${TMAN_PREFIX}/tasks/${TMAN_CURR}"
        cd "$taskdir" || return 1
        _tman_wd_add task

    elif [ "$cmd" = "del" ]; then
        local taskdir="${TMAN_PREFIX}/tasks/${TMAN_CURR}"
        cd "$taskdir" || return 1
        if [ -n "$taskid" ]; then
            _tman_wd_add task
        else
            _tman_wd_rm task
        fi

    elif [ "$cmd" = "prev" ]; then
        local taskdir="${TMAN_PREFIX}/tasks/${TMAN_CURR}"
        cd "$taskdir" || return 1
        _tman_wd_add task

    #elif [ "$cmd" = "env" ] && [ "$2" = "prev" ]; then
    elif [ "$cmd" = "env" ]; then
        # load updated sys.conf values
        _tman_get_sys_config_vars
        _tman_form_full_command

        local taskdir="${TMAN_PREFIX}/tasks/${TMAN_CURR}"
        cd "$taskdir" || return 1
        _tman_wd_add task

    elif [ "$cmd" = "set" ]; then
        # raach: Refactor it. Too complicated.
        local myargs=("$@")
        local myargs=("${myargs[@]:1}")
        local flag=false

        while getopts ":i" arg "${myargs[@]}";  do
            case "$arg" in
                i)
                    flag=true
                    ;;
                *)
                    ;;
            esac
        done

        if [ "$flag" = true ]; then
            local taskdir="${TMAN_PREFIX}/tasks/${TMAN_CURR}"

            cd "$taskdir" || return 1
            _tman_wd_add task

            # switch back if CWD is not the task ID changed.
            if [ -d "$TMAN_CWD" ]; then
                cd "$TMAN_CWD" || return 1
            fi
        fi

    elif [ "$cmd" = "sync" ]; then
        local taskdir="${TMAN_PREFIX}/tasks/${TMAN_CURR}"
        cd "$taskdir" || return 1
        _tman_wd_add task

    elif [ "$cmd" = "use" ]; then
        local taskdir="${TMAN_PREFIX}/tasks/${TMAN_CURR}"
        cd "$taskdir" || return 1
        _tman_wd_add task
    fi
}

function _tman_form_full_command()
{
    local script="${TMAN_INSTALL}/src/core/tman.lua"

    local stat="package.path = package.path"
    stat="$stat .. ';${TMAN_INSTALL}/src/?.lua;'"
    stat="$stat .. ';${TMAN_INSTALL}/src/aux/?.lua;'"
    stat="$stat .. ';${TMAN_INSTALL}/src/core/?.lua;'"
    stat="$stat .. ';${TMAN_INSTALL}/src/plugin/?.lua;'"
    stat="$stat .. ';${TMAN_INSTALL}/src/builtin/?.lua;'"

    TMAN="lua -e \"$stat\" $script"
}

function tman()
{
    local retcode=

    _tman_get_sys_config_vars
    _tman_form_full_command

    TMAN_CWD="$(pwd)"

    eval "$TMAN" "$*"
    retcode="$?"

    if [ $retcode -ne 0 ]; then
        return $retcode
    fi

    _tman_handle_command "$@"
}
