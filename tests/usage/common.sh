#!/bin/env bash

HOOKTOG="off"
DEBUGTOG="off"
TASKDIR="/tmp/tectask-test"
PWDFILE="/tmp/tecpwd" # TODO: use m4 to generate path

TECCLI="teccli -H $HOOKTOG -D $DEBUGTOG -T $TASKDIR"

function tec_cleanup()
{
    tec_deinit
}

function tec_init()
{
    eval "$TECCLI" init
}

function tec_deinit()
{
    rm -rf "$TASKDIR"
}

