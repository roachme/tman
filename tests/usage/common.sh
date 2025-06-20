#!/bin/env bash

HOOKTOG="off"
DEBUGTOG="off"
TASKDIR="/tmp/tmantask-test"
PWDFILE="/tmp/tmanpwd" # TODO: use m4 to generate path

TMANCLI="_tmancli -H $HOOKTOG -D $DEBUGTOG -T $TASKDIR"

function tman_cleanup()
{
    tman_deinit
}

function tman_init()
{
    eval "$TMANCLI" init
}

function tman_deinit()
{
    rm -rf "$TASKDIR"
}

