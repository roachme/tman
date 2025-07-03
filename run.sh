TASKDIR="$HOME/trash/tman"
PRJNAME="test"
BRDNAME="brd-000"
TASKID="test1"


rm -rf "$TASKDIR"


make; echo; echo

./_tmancli -T "$TASKDIR" -H off init
./_tmancli -T "$TASKDIR" -H off prj add -n "$PRJNAME"
#./_tmancli -T "$TASKDIR" -H off prj list
./_tmancli -T "$TASKDIR" -H off brd add -n -p "$PRJNAME" "$BRDNAME"

echo

#ls -la "$TASKDIR/$PRJNAME"
find "$TASKDIR/$PRJNAME"
