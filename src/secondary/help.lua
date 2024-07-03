--- Provide help on commands and general usage.
-- @module help

local version = "0.1.14"
local progname = "tman"

local function show_usage()
    io.stdout:write(([[
Usage: %s COMMAND [OPTION] [ID]

List of available commands:

System:
  archive - backup and restore metadata
  config  - config your workflow
  get     - get parameters like curr, prev tasks, size, etc
  init    - init util structure

Basic:
  cat     - cat task info
  env     - define or display environments
  list    - list tasks
  prev    - switch to previous task
  use     - mark a task as current

Change:
  add     - add new task
  del     - delete task
  pack    - pack commits in repos for review.
  set     - set task units
  sync    - synchronize task struct, git branch, repos, etc

Info:
  help    - display help information about %s
  ver     - show version

'%s help COMMAND' to get detailed info about command.
'%s help %s' to get info about util itself.
]]):format(progname, progname, progname, progname, progname))
end

local cmds = {
    {
        name = progname,
        desc = [[
tman - terminal task manager.

Util to switch between tasks, git branches and make the workflow easy and
less confusing. You no longer need to memerize task IDs, branche names,
cd between directories, lose task notes, but instead look
for 'em in nice structured way.
]],
    },
    {
        name = "init",
        desc = [[
Usage: tman init
Initialize util.
]],
    },
    {
        name = "archive",
        desc = [[
Usage: tman archive OPTION
Make backup and restore util data.

Options:
    -R          include repos into archive (default: false)
    -b FILE     create backup (default extension: .tar)
    -h          Show this help message.
    -r FILE     restore from archive
]],
    },
    {
        name = "config",
        desc = [[
Usage: tman config OPTION
Work with config files.

Options:
    -b      - set base path. Prefix is user home.
    -i      - set install path
              questionable, cuz util gotta change path in .shellrc
    -s      - show config: system & user
]],
    },
    {
        name = "add",
        desc = [[
Usage: tman add [OPTION] ID
Add new task.

Options:
    -d      set task description (under development)
    -h      show this help message.
    -p      set task priority. Values: [highest|high|mid|low|lowest]
    -t      set task type. Values: [bugfix|hotfix|feature]
]],
    },
    {
        name = "del",
        desc = [[
Usage: tman del [ID]
Delete task.
ID - default is current task.
]],
    },
    {
        name = "set",
        desc = [[
Usage: tman set OPTION [ID]
Amend task items.
ID - default is current task.

Options:
    -i      set task ID
    -d      set task description
    -l      set task link
    -p      set task priority. Values: [highest|high|mid|low|lowest]
    -t      set task type. Values: [bugfix|hotfix|feature]
]],
    },
    {
        name = "sync",
        desc = [[
Usage: tman sync OPTION [ID]
Update task repos, structure, etc. Default: current task.
With no option applied jump to task directory.

Notes: Add support for other task cuz another task might be broken so
      repair's need (if task branch's deleted)

Options:
    -s      - synchronize task structure
    -r      - git pull from remote repo
    -t      - synchronize task status (active, completed)
]],
    },

    {
        name = "use",
        desc = [[
Usage: tman use ID
Switch to specified task. Use `tman list` to see existing tasks.

Notes:
    ID - taks ID. Default is current task.
]],
    },
    {
        name = "get",
        desc = [[
Usage: tman get PARAM
Get parameters like curr, prev tasks, current env, etc

Notes:
PARAM can have one of the next values
    env     - current env
    curr    - current task ID
    prev    - previous task ID
]],
    },
    {
        name = "env",
        desc = [[
Usage: tman env COMMAND [ENVNAME]
Define or display environments.

ENVNAME - environment name

COMMANDs:
    add     - add new environment
    curr    - show current environment
    del     - delete an environment
    list    - list environments
    prev    - switch to previous environment
    use     - use environment
]],
    },
    {
        name = "prev",
        desc = [[
Usage: tman prev [ENVNAME]
Switch to previous task if set.

ENVNAME - environment name. Under development (pro'ly).

Options:
    -h   Show this help message.
]],
    },
    {
        name = "pack",
        desc = [[
Usage: tman pack OPTION [ID]
Pack commits in repos for review.

Options:
    -c      - create commit (default)
    -m      - run commands from the Makefile
    -p      - push branch to remote repo
]],
    },
    {
        name = "list",
        desc = [[
Usage: tman list [OPTION]
List task IDs with description.

Options:
    -c   List only complete tasks.
    -a   List only active tasks (default).
    -A   List all tasks: active and complete.
    -h   Show this help message.

Notes:
    *   Marks current task.
    -   Makrs previous task.
]],
    },
    {
        name = "cat",
        desc = [[
Usage: tman cat [OPTION] [ID]
Show task units.
ID - task ID (default is current task).

Options:
    -k   cat specific task unit
    -h   Show this help message.
]],
    },
    {
        name = "help",
        desc = [[
Usage: tman help [COMMAND]
Show help message. If `COMMAND' applied then show info about command.
]],
    },
    {
        name = "ver",
        desc = [[
Usage: tman ver
Show tman version.
]],
    },
}

--- Get general and detailed info about command.
-- @param cmdname command name to get info about
-- @return status code: 0 - ok, otherwise error code
local function help_usage(cmdname)
    local errmsg = "%s: no such command '%s'. Use '%s help' for more info.\n"

    if not cmdname then
        show_usage()
        return 0
    end

    for _, cmd in ipairs(cmds) do
        if cmd.name == cmdname then
            io.stdout:write(cmd.desc)
            return 0
        end
    end
    io.stderr:write(errmsg:format(progname, cmdname, progname))
    return 1
end

return {
    usage = help_usage,
    version = version,
    progname = progname,
}
