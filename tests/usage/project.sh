#!/bin/env bash

source tests/usage/common.sh


## Command add: START
function tec_add_project_existing_project()
{
    local prjname="validprj1"

    eval "$TECCLI" prj add "$prjname" 2>/dev/null
    eval "$TECCLI" prj add "$prjname" 2>/dev/null
    if [ "$?" -eq 0 ]; then
        echo "FAIL: tec_add_project_existing_project"
        return 1
    fi
    echo "OK: tec_add_project_existing_project"
}

function tec_add_project_with_valid_name()
{
    local prjname="validprj2"

    eval "$TECCLI" prj add "$prjname" 2>/dev/null
    if [ "$?" -ne 0 ]; then
        echo "FAIL: tec_add_project_with_valid_name"
        return 1
    fi
    echo "OK: tec_add_project_with_valid_name"
}

function tec_add_project_with_invalid_name()
{
    local prjname="in{vaild3"

    eval "$TECCLI" prj add "$prjname" 2>/dev/null
    if [ "$?" -eq 0 ]; then
        echo "FAIL: tec_add_project_with_invalid_name"
        return 1
    fi
    echo "OK: tec_add_project_with_invalid_name"
}

function tec_add_project_with_valid_name_with_pwd()
{
    local prjname="validprj4"
    local pwdcontent

    eval "$TECCLI" prj add "$prjname" 2>/dev/null
    if [ "$?" -ne 0 ]; then
        echo "FAIL: tec_add_project_with_valid_name: could not create"
        echo "  expect: '$TASKDIR/$prjname'"
        echo "  result: '$pwdcontent'"
        return 1
    fi

    pwdcontent="$(cat "$PWDFILE")"
    if [ "$pwdcontent" != "$TASKDIR/$prjname/" ]; then
        echo "FAIL: tec_add_project_with_valid_name: invalid pwd"
        echo "  expect: '$TASKDIR/$prjname'"
        echo "  result: '$pwdcontent'"
        return 1
    fi
    echo "OK: tec_add_project_with_valid_name_with_pwd"
}
## Command add: END

# Test driver
tec_cleanup
tec_init

# Test cases
tec_add_project_existing_project
tec_add_project_with_valid_name
tec_add_project_with_invalid_name
tec_add_project_with_valid_name_with_pwd

tec_deinit
