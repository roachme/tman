#!/bin/env bash


PGNAME="pgm"
URL="https://github.com/roachme/tman-pgm"
# TODO: Get it either from opiton or load it from config file
PGNDIR="$HOME/.local/lib/tman/pgn"


if [ -d "${PGNDIR}/${PGNAME}" ]; then
    echo "already installed"
else
    git clone --recurse-submodules "$URL" "${PGNDIR}/${PGNAME}"
fi

