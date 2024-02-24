function tman()
{
    CWD=$(pwd)
    TASKS="$HOME/work/tasks"
    CURRTASK=$(cat "$HOME/work/tasks/.curr")
    cd ${HOME}/personal/prjs/tman
    lua taskman.lua $@
    RET=$?

    # tman new DE-me
    # tman use DE-me
    # tman prev
    if [ $RET -eq 0 ] && [ "$1" = "new" ]; then
        cd $TASKS/${2}
        wd -q rm task
        wd -q add task
        wd task

    elif [ $RET -eq 0 ] && [ "$1" = "use" ]; then
        cd $TASKS/${2}
        wd -q rm task
        wd -q add task
        wd task

    elif [ $RET -eq 0 ] && [ "$1" = "prev" ]; then
        PREVID=$(lua taskman.lua getprev)
        cd $TASKS/${PREVID}
        wd -q rm task
        wd -q add task
        wd task

    elif [ $RET -eq 0 ] && [ "$1" = "move" ]; then
        if [ ! -z "$3" ] && [ "$2" = "progress" ]; then
            cd $TASKS/${3}
            wd -f -q add task
            wd task
        fi

    elif [ "$1" = "del" ]; then
        cd $TASKS

    elif [ $RET -ne 0 ]; then
        echo $CURRTASK > $CURRTASK

    else
        cd $CWD
    fi
}
