TASKDIR="$HOME/trash/tman"
PRJNAME="test"
BRDNAME="brd-000"
TASKID="test1"


if [ "$1" = "prj" ]; then
    find "$TASKDIR" -maxdepth 2 -name '.tman' -type d | tail -n +2  | while read -r dir; do
        echo "$(basename ${dir/.tman/}): $(cat $dir/column)"
    done
elif [ "$1" = "brd" ]; then
    find "$TASKDIR" -mindepth 3 -maxdepth 3 -name '.tman' -type d | while read -r dir; do
        echo "$(basename ${dir/.tman/}): $(cat $dir/column)"
    done
elif [ "$1" = "task" ]; then
    find "$TASKDIR" -mindepth 4 -name '.tman' -type d | while read -r dir; do
        echo "$(basename ${dir/.tman/}): $(cat $dir/column)"
    done
fi
