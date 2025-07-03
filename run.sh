TASKDIR="$HOME/trash/tman"
PRJNAME="test"
BRDNAME="brd-000"
TASKID="test1"


rm -rf "$TASKDIR"


make; echo; echo

./_tmancli -T "$TASKDIR" init
./_tmancli -T "$TASKDIR" prj add -n "$PRJNAME"
#./_tmancli -T "$TASKDIR" prj list
./_tmancli -T "$TASKDIR" brd add -n -p "$PRJNAME" "$BRDNAME"

echo

#ls -la "$TASKDIR/$PRJNAME"
find "$TASKDIR/$PRJNAME"
