#!/bin/bash

TMAN=""
TMAN_WD=
TMAN_CWD=

# config vars
TMAN_ENV=
TMAN_PREFIX=
TMAN_INSTALL=

TMAN_TMANCONF=


# tman -b basepath -c configpath CMD OPTIONS arg


# TODO: switch to current task ID when change env

function _tman_get_tmanconf()
{
    TMAN_TMANCONF="${HOME}/.config/tman/sys.conf"
}

function _tman_get_sys_config_vars()
{
    _tman_get_tmanconf
    TMAN_ENV="$(grep env "$TMAN_TMANCONF" | cut -f 2 -d '=' | tr -d ' ' | tr -d '"' | tr -d "'")"
    TMAN_PREFIX="$(grep prefix "$TMAN_TMANCONF" | cut -f 2 -d '=' | tr -d ' ' | tr -d '"' | tr -d "'")"
    TMAN_INSTALL="$(grep install "$TMAN_TMANCONF" | cut -f 2 -d '=' | tr -d ' ' | tr -d '"' | tr -d "'")"
}

function _tman_handle_command()
{
    local cmd="$1"
    local taskid=
    local taskdir=

    # roachme:TODO: it's a big permormance issue, it executes tman command twice
    # roachme: should've used TMAN_ENV

    if [ "$cmd" = "add" ]; then
        taskid="${*: -1}" # get last argument
        local taskdir="${TMAN_PREFIX}/${TMAN_ENV}/tasks/${taskid}"
        cd "$taskdir" || return 1
        wd add -q -f task

    elif [ "$cmd" = "del" ]; then
        taskid="$(eval "$TMAN" get curr)"
        taskdir="${TMAN_PREFIX}/${TMAN_ENV}/tasks/${taskid}"
        cd "$taskdir" || return 1
        if [ -n "$taskid" ]; then
            wd add -q -f task
        else
            wd rm -q task
        fi

    elif [ "$cmd" = "prev" ]; then
        taskid="$(eval "$TMAN" get curr)"
        taskdir="${TMAN_PREFIX}/${TMAN_ENV}/tasks/${taskid}"
        cd "$taskdir" || return 1
        wd add -q -f task

    #elif [ "$cmd" = "env" ] && [ "$2" = "prev" ]; then
    elif [ "$cmd" = "env" ]; then
        # load updated sys.conf values
        _tman_get_sys_config_vars
        _tman_form_full_command

        taskid="$(eval "$TMAN" get curr)"
        taskdir="${TMAN_PREFIX}/${TMAN_ENV}/tasks/${taskid}"
        cd "$taskdir" || return 1
        wd add -q -f task

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
            taskid="$(eval "$TMAN" get curr)"
            taskdir="${TMAN_PREFIX}/${TMAN_ENV}/tasks/${taskid}"

            cd "$taskdir" || return 1
            wd add -q -f task

            # switch back if CWD is not the task ID changed.
            if [ -d "$TMAN_CWD" ]; then
                cd "$TMAN_CWD" || return 1
            fi
        fi

    elif [ "$cmd" = "sync" ]; then
        taskid="$(eval "$TMAN" get curr)"
        taskdir="${TMAN_PREFIX}/${TMAN_ENV}/tasks/${taskid}"
        cd "$taskdir" || return 1
        wd add -q -f task

    elif [ "$cmd" = "use" ]; then
        taskid="$2"
        taskdir="${TMAN_PREFIX}/${TMAN_ENV}/tasks/${taskid}"
        cd "$taskdir" || return 1
        wd add -q -f task
    fi
}

function _tman_form_full_command()
{
    local script="${TMAN_INSTALL}/src/core/tman.lua"

    local stat="package.path = package.path"
    stat="$stat .. ';${TMAN_INSTALL}/src/?.lua;'"
    stat="$stat .. ';${TMAN_INSTALL}/src/aux/?.lua;'"
    stat="$stat .. ';${TMAN_INSTALL}/src/core/?.lua;'"
    stat="$stat .. ';${TMAN_INSTALL}/src/misc/?.lua;'"
    stat="$stat .. ';${TMAN_INSTALL}/src/builtin/?.lua;'"
    stat="$stat .. '${HOME}/.config/tman/?.lua'"

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
