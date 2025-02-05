tags=( env_init env_reset env_getcurr env_getprev env_delcurr env_delprev env_swapenvs env_addcurr env_exists env_isvalid env_iscurr env_isprev)



for tag in ${tags[@]}; do
    SRC="$tag"
    DST="$(echo $tag | sed 's/env/prj/g')"
    find . -name '*.[h,c]' | xargs sed -i "s/$SRC/$DST/g"
done
