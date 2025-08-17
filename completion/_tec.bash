function _comp_tec()
{
    local cur
    declare -A CMD_DESC=(
        ["help"]="Show help message for command"
        ["init"]="Init directory structure"

        ["add"]="Add task to project"
        ["del"]="Delete task from project"
        ["list"]="List project tasks"
        ["move"]="Move (rename) tasks"
        ["set"]="Set task unit values"
        ["show"]="Show task unit values"
        ["sync"]="Switch or synchronize task"

        ["board"]="Manage and show boards"
        ["project"]="Manage and show projects"
    )
    declare -A UTIL_OPT=(
        ["-h"]="display help information"
        ["-C"]="use colors"
        ["-H"]="use hooks"
        ["-V"]="display version info"
    )

    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    echo
    echo "cur: '$cur'"
    echo "COMP_CWORD: $COMP_CWORD"

    if [ "$COMP_CWORD" -eq 1 ] && [ "${cur:0:1}" == '-' ]; then
        for option in "${!UTIL_OPT[@]}"; do
            printf "%s\t\t -- %s\n" "$option" "${UTIL_OPT[$option]}"
        done
    elif [ "$COMP_CWORD" -ge 1 ] && [ -z "$cur" ]; then
        for desc in "${!CMD_DESC[@]}"; do
            printf "%s\t\t -- %s\n" "$desc" "${CMD_DESC[$desc]}"
        done
    fi
    return 0;
}

complete -F _comp_tec tec
