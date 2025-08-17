#compdef tec

# TODO:
# 1. Add support to suggest objects (project, board, task ID)
# 2. Add support for toggles in options -C, -D, -H, etc

local -a subcommands global_opts

global_opts=(
    '(-C)'{-C,--color}'[enable colors]:toggle:(on off)'
    '(-D)'{-D,--debug}'[enable debug mode]:toggle:(on off)'
    '(-h)'{-h,--help}'[show help and exit]'
    '(-F)'{-F,--config}'[path to config file]:directory:_files'
    '(-H)'{-H,--hooks}'[enable hooks]:toggle:(on off)'
    '(-P)'{-P,--plugins}'[plugins directory]:directory:_files'
    '(-T)'{-T,--tasks}'[tasks directory]:directory:_files'
    '(-V)'{-V,--version}'[show version and exit]'
)

_subcommands() {
    subcommands=(
        'help:Show help for commands'
        'init:Init directory structure'
        'add:Add a new task to project'
        'del:Delete task from project'
        'list:List project tasks'
        'move:Move (rename) tasks'
        'prev:Switch to previous task'
        'set:Set task unit values'
        'show:Show task unit values'
        'sync:Synchronize (with) task'
        'board:Manage and show boards'
        'column:Manage and show columns'
        'project:Manage and show projects'
    )
}

_tec_help() {
    _arguments \
        '(-d)'{-d,--desc}'[output short description for each topic]' \
        '(-s)'{-s,--synopsis}'[output only a short usage synopsis]' \
        ':command:->command'

    case $state in
        command)
            local -a help_topics
            help_topics=(${(k)_tec_subcommands} ${(k)_tec_board_subcommands} ${(k)_tec_column_subcommands} ${(k)_tec_project_subcommands})
            _describe 'help topic' help_topics
            ;;
    esac
}

_tec_add() {
    _arguments \
        '(-b)'{-b,--board}'[board name]:board:_tec_boards' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-n)'{-n,--no-switch}'[do not switch to task]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '(-N)'{-N,--no-switch-dir}'[neither switch to task nor to task directory]' \
        '*:task ID:'
}

_tec_del() {
    _arguments \
        '(-b)'{-b,--board}'[board name]:board:_tec_boards' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '(-y)'{-y,--yes}'[delete without confirmation]' \
        '*:task ID:_tec_tasks'
}

_tec_list() {
    _arguments \
        '(-a)'{-a,--all}'[list all tasks]' \
        '(-b)'{-b,--board}'[board name]:board:_tec_boards' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '(-v)'{-v,--verbose}'[show more verbose output]' \
        '(-t)'{-t,--toggles}'[show only toggle switches]' \
        '(-H)'{-H,--headers}'[show headers]' \
        '*:project:_tec_projects'
}

_tec_move() {
    _arguments \
        '(-d)'{-d,--destination}'[destination project]:project:_tec_projects' \
        '(-f)'{-f,--force}'[overwrite destination task]' \
        '(-s)'{-s,--source}'[source project]:project:_tec_projects' \
        '(-T)'{-T,--tasks}'[treat all IDs as source to move to project]' \
        '*:task ID:_tec_tasks'
}

_tec_prev() {
    _arguments \
        '(-b)'{-b,--board}'[board name]:board:_tec_boards' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]'
}

_tec_set() {
    _arguments \
        '(-b)'{-b,--board}'[board name]:board:_tec_boards' \
        '(-d)'{-d,--description}'[task description]' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '(-t)'{-t,--type}'[task type]:type:(task bugfix feature hotfix)' \
        '(-P)'{-P,--priority}'[task priority]:priority:(lowest low mid high highest)' \
        '*:task ID:_tec_tasks'
}

_tec_show() {
    _arguments \
        '(-b)'{-b,--board}'[board name]:board:_tec_boards' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-k)'{-k,--key}'[key to show]:key' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '*:task ID:_tec_tasks'
}

_tec_sync() {
    _arguments \
        '(-b)'{-b,--board}'[board name]:board:_tec_boards' \
        '(-n)'{-n,--no-update}'[do not update toggles]' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '(-N)'{-N,--no-switch-dir}'[neither update toggles nor switch to task directory]' \
        '*:task ID:_tec_tasks'
}

# Board subcommands
_tec_board() {
    local curcontext="$curcontext" state line
    typeset -A opt_args

    _arguments -C \
        '(-h)'{-h,--help}'[show help and exit]' \
        ':subcommand:->subcmd' \
        '*:: :->args'

    case $state in
        subcmd)
            local -a board_subcommands
            board_subcommands=(
                'add:Add a new board'
                'del:Delete board with all tasks'
                'list:List boards'
                'move:Move (rename) boards'
                'prev:Switch to previous board'
                'set:Set board values'
                'show:Show board info'
                'sync:Switch to or synchronize (with) board'
            )
            _describe -t commands 'board subcommand' board_subcommands
            ;;
        args)
            case $line[1] in
                add) _tec_board_add ;;
                del) _tec_board_del ;;
                list) _tec_board_list ;;
                move) _tec_board_move ;;
                prev) _tec_board_prev ;;
                set) _tec_board_set ;;
                show) _tec_board_show ;;
                sync) _tec_board_sync ;;
            esac
            ;;
    esac
}

_tec_board_add() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-n)'{-n,--no-switch}'[do not switch to new board]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '*:board name:'
}

_tec_board_del() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-n)'{-n,--no-confirm}'[delete without confirmation]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '*:board name:_tec_boards'
}

_tec_board_list() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        ':project:_tec_projects'
}

_tec_board_move() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        ':source board:_tec_boards' \
        ':destination board:'
}

_tec_board_prev() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]'
}

_tec_board_set() {
    _arguments \
        '(-d)'{-d,--description}'[board description]' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '*:board name:_tec_boards'
}

_tec_board_show() {
    _arguments \
        '(-b)'{-b,--board}'[board name]:board:_tec_boards' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-k)'{-k,--key}'[key to show]:key' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '*:board name:_tec_boards'
}

_tec_board_sync() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-n)'{-n,--no-switch}'[do not switch to board]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '*:board name:_tec_boards'
}

# Column subcommands
_tec_column() {
    local curcontext="$curcontext" state line
    typeset -A opt_args

    _arguments -C \
        '(-h)'{-h,--help}'[show help and exit]' \
        ':subcommand:->subcmd' \
        '*:: :->args'

    case $state in
        subcmd)
            local -a column_subcommands
            column_subcommands=(
                'list:List columns'
                'move:Move task to column'
            )
            _describe -t commands 'column subcommand' column_subcommands
            ;;
        args)
            case $line[1] in
                list) _tec_column_list ;;
                move) _tec_column_move ;;
            esac
            ;;
    esac
}

_tec_column_list() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]'
}

_tec_column_move() {
    _arguments \
        '(-b)'{-b,--board}'[board name]:board:_tec_boards' \
        '(-c)'{-c,--column}'[column to move to]:column:_tec_columns' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '(-p)'{-p,--project}'[project name]:project:_tec_projects' \
        '*:task ID:_tec_tasks'
}

# Project subcommands
_tec_project() {
    local curcontext="$curcontext" state line
    typeset -A opt_args

    _arguments -C \
        '(-h)'{-h,--help}'[show help and exit]' \
        ':subcommand:->subcmd' \
        '*:: :->args'

    case $state in
        subcmd)
            local -a project_subcommands
            project_subcommands=(
                'add:Add a new project'
                'del:Delete project with all boards and tasks'
                'list:List projects'
                'prev:Switch to previous project'
                'rename:Rename project'
                'set:Set project values'
                'show:Show project info'
                'sync:Switch to or synchronize (with) project'
            )
            _describe -t commands 'project subcommand' project_subcommands
            ;;
        args)
            case $line[1] in
                add) _tec_project_add ;;
                del) _tec_project_del ;;
                list) _tec_project_list ;;
                prev) _tec_project_prev ;;
                rename) _tec_project_rename ;;
                set) _tec_project_set ;;
                show) _tec_project_show ;;
                sync) _tec_project_sync ;;
            esac
            ;;
    esac
}

_tec_project_add() {
    _arguments \
        '(-b)'{-b,--board}'[board name]:board:_tec_boards' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-n)'{-n,--no-switch}'[do not switch to new project]' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '*:project name:'
}

_tec_project_del() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-n)'{-n,--no-confirm}'[delete without confirmation]' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '*:project name:_tec_projects'
}

_tec_project_list() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '(-v)'{-v,--verbose}'[show more info]'
}

_tec_project_prev() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]'
}

_tec_project_rename() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        ':source project:_tec_projects' \
        ':destination project:'
}

_tec_project_set() {
    _arguments \
        '(-d)'{-d,--description}'[project description]' \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '*:project name:_tec_projects'
}

_tec_project_show() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '*:project name:_tec_projects'
}

_tec_project_sync() {
    _arguments \
        '(-h)'{-h,--help}'[show help and exit]' \
        '(-n)'{-n,--no-switch}'[do not switch to project]' \
        '(-q)'{-q,--quiet}'[do not write to stderr]' \
        '*:project name:_tec_projects'
}

# Helper functions
_tec_projects() {
    local -a projects
    # This would ideally query the actual projects, but for completion we'll just provide a stub
    projects=('project1' 'project2' 'project3')
    _describe 'project' projects
}

_tec_boards() {
    local -a boards
    # Similarly, this would query the actual boards
    boards=('board1' 'board2' 'board3')
    _describe 'board' boards
}

_tec_columns() {
    local -a columns
    columns=('todo' 'in-progress' 'done' 'review')
    _describe 'column' columns
}

_tec_tasks() {
    local -a tasks
    # This would query the actual tasks
    tasks=('task1' 'task2' 'task3')
    _describe 'task' tasks
}

# Main completion function
_tec() {
    local curcontext="$curcontext" state line
    typeset -A opt_args

    _arguments -C -s \
        $global_opts \
        ':subcommand:->subcmd' \
        '*:: :->args'

    case $state in
        subcmd)
            _subcommands
            _describe -t commands 'tec command' subcommands
            ;;
        args)
            case $line[1] in
                help) _tec_help ;;
                init) ;;
                add) _tec_add ;;
                del) _tec_del ;;
                list) _tec_list ;;
                move) _tec_move ;;
                prev) _tec_prev ;;
                set) _tec_set ;;
                show) _tec_show ;;
                sync) _tec_sync ;;
                board) _tec_board ;;
                column) _tec_column ;;
                project) _tec_project ;;
            esac
            ;;
    esac
}

_tec "$@"
