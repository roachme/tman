#!/bin/env bash

source tests/usage/common.sh


## Command add: START
function tman_add_project_existing_project()
{
    local prjname="validprj1"

    eval "$TMANCLI" prj add "$prjname" 2>/dev/null
    eval "$TMANCLI" prj add "$prjname" 2>/dev/null
    if [ "$?" -eq 0 ]; then
        echo "FAIL: tman_add_project_existing_project"
        return 1
    fi
    echo "OK: tman_add_project_existing_project"
}

function tman_add_project_with_valid_name()
{
    local prjname="validprj2"

    eval "$TMANCLI" prj add "$prjname" 2>/dev/null
    if [ "$?" -ne 0 ]; then
        echo "FAIL: tman_add_project_with_valid_name"
        return 1
    fi
    echo "OK: tman_add_project_with_valid_name"
}

function tman_add_project_with_invalid_name()
{
    local prjname="in{vaild3"

    eval "$TMANCLI" prj add "$prjname" 2>/dev/null
    if [ "$?" -eq 0 ]; then
        echo "FAIL: tman_add_project_with_invalid_name"
        return 1
    fi
    echo "OK: tman_add_project_with_invalid_name"
}

function tman_add_project_with_valid_name_with_pwd()
{
    local prjname="validprj4"
    local pwdcontent

    eval "$TMANCLI" prj add "$prjname" 2>/dev/null
    if [ "$?" -ne 0 ]; then
        echo "FAIL: tman_add_project_with_valid_name: could not create"
        echo "  expect: '$TASKDIR/$prjname'"
        echo "  result: '$pwdcontent'"
        return 1
    fi

    pwdcontent="$(cat "$PWDFILE")"
    if [ "$pwdcontent" != "$TASKDIR/$prjname/" ]; then
        echo "FAIL: tman_add_project_with_valid_name: invalid pwd"
        echo "  expect: '$TASKDIR/$prjname'"
        echo "  result: '$pwdcontent'"
        return 1
    fi
    echo "OK: tman_add_project_with_valid_name_with_pwd"
}
## Command add: END

# Test driver
tman_cleanup
tman_init

# Test cases
tman_add_project_existing_project
tman_add_project_with_valid_name
tman_add_project_with_invalid_name
tman_add_project_with_valid_name_with_pwd

tman_deinit
