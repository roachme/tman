#!/bin/bash

usage()
{
    cat << EOF
Usage: ./install.sh [USERTYPE] [OPTION]

USERTYPE - install for user or developer

Options:
    -c      check system utils and luarocks (default).
    -i      user env: install only necessary dependencies for util to work
    -I      developer env: install system utils and luarocks
    -g      generate and install system config
    -h      show this help message
EOF
}

generate_system_config()
{
    # roachme: User defined pathes.
    # Add flags to change 'em from script interface.
    local fenv="${HOME}/.config/tman/env.list"
    local confdir="${HOME}/.config/tman"
    local prefix="${HOME}/work"
    local tmanbase="tman"
    local tmancore="${tmanbase}/.tman"

    local tmaninst="$(pwd)" # roachme: pro'ly change it to ~/.local/bin
    local tmanconf="$confdir/sys.conf"
    local userconf="$confdir/tman_conf.lua" # FIXME: should be user.conf

    # create tman base directory
    mkdir -p "$prefix/$tmanbase"

    # create tman core directory (core migth be inside base)
    mkdir -p "$prefix/$tmancore"

    # create system and user configs
    mkdir -p "$confdir"
    touch "$tmanconf"
    touch "$userconf"

    # fill tman system config
    echo "# NOT recommended to change this file manually." > "$tmanconf"
    echo "prefix = $prefix" >> "$tmanconf"
    echo "install = $tmaninst" >> "$tmanconf"

    # fill tman user config
    echo "return {}" > "$userconf"

    # fill default env name
    echo "0 work: main work env" > "$fenv"

    # create task ID database file
    touch "$prefix/$tmancore/taskids"

    # create task units database directory
    mkdir -p "$prefix/$tmancore/units"
}

check_system_utils()
{
    local system_utils=("tar" "luarocks" "cargo")
    for sutil in ${system_utils[@]}; do
        which -s $sutil > /dev/null
        if [ $? -ne 0 ]; then
            echo "- System util '$sutil' is missing"
        fi
    done
    echo "check: System utils is done" && echo
}

check_lua_rocks()
{
    local lua_rocks=("luaposix" "cjson")
    for lutil in ${lua_rocks[@]}; do
        luarocks list | grep -q "$lutil"
        if [ $? -ne 0 ]; then
            echo "- Luarock '$lutil' is missing"
        fi
    done
    echo "check: Lua rocks check is done" && echo
}

install_lua_rocks()
{
    local lua_rocks=("luaposix" "cjson")
    for lutil in ${lua_rocks[@]}; do
        echo "- install Lua rock: $lutil"
        luarocks install $lutil
    done
    echo "Necessary Lua rocks installed"
}

install_system_utils()
{
    local system_utils="tar luarocks"
    sudo apt update
    sudo apt install -y ${system_utils}
}

# TODO: add a checker to not install it more than once
install_shell()
{
    echo "Add: 'source $(pwd)/tman.sh' into yer shell config .bashrc or .zshrc"
    echo "And restart your current shell and execute 'tman init'."

    return 1
    # env SHELL might be not set
    #local USERSHELL="$(basename $(getent passwd $(whoami) | awk -F: '{print $7}'))"
#    local bashpath
#    local USERSHELL
#
#    bashpath="$(readlink /proc/$$/exe)"
#    USERSHELL="$(basename "${bashpath}")"
#
#    if [ "$USERSHELL" == "bash" ]; then
#        echo "install into bash"
#    elif [ "$USERSHELL" == "zsh" ]; then
#        echo "install into zsh"
#    else
#        echo "Unsupported shell '$USERSHELL'"
#        exit 1
#    fi
#
#
#    echo "source $(pwd)/tman.sh" >> "${HOME}/.${USERSHELL}rc"
#    echo
#    echo "WARNING: 'source ~/$USERSHELL' - to restart shell"
}


if [ "$1" = "-i" ]; then
    install_system_utils
    install_lua_rocks
    install_shell
    generate_system_config
elif [ "$1" = "-I" ]; then
    install_system_utils
    install_lua_rocks
    install_shell
    cargo install stylua # roachme: find a better way
elif [ "$1" = "-c" ]; then
    check_system_utils
    check_lua_rocks
elif [ "$1" = "-g" ]; then
    generate_system_config
elif [ "$1" = "-h" ]; then
    usage
elif [ -n "$1" ]; then
    echo "unknown option '${1}'. Use option '$0 -h' to get some help."
else
    echo "missing option. Use option '$0 -h' to get some help."
fi
