local env = require("core.env")
local git = require("core.git")
local taskid = require("core.taskid")
local struct = require("core.struct")
local taskunit = require("core.taskunit")
local common = require("core.common")
local help = require("core.help")
local getopt = require("posix.unistd").getopt
local config = require("core.config")

--- Add a new task.
-- Fill the rest with default values.
-- @see tman_set
-- @return on success - true
-- @return on failrue - false
local function builtin_add()
    local id
    local cmdname = "add"
    local prio = "mid"
    local tasktype = "bugfix"
    -- roachme: move `-d' option from taskunit.lua to here.
    local optstr = "hp:t:"
    local last_index = 1
    local keyhelp
    local envname = env.getcurr()
    --envname = "test"

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            common.die(1, "unrecognized option\n", arg[optind - 1])
        end

        last_index = optind
        if optopt == "h" then
            keyhelp = true
        elseif optopt == "p" then
            prio = optarg
        elseif optopt == "t" then
            tasktype = optarg
        end
    end

    if keyhelp then
        help.usage(cmdname)
        -- roachme:hotfix: so tman.sh doesn't jump to any task directory.
        return 1
    end

    id = arg[last_index]

    if not git.branch_isuncommited() then
        -- roachme: would be nice to know what repo.
        io.stderr:write("repo has uncommited changes\n")
        os.exit(1)
    end
    if not id then
        io.stderr:write("task ID required\n")
        os.exit(1)
    end

    taskid.init(config.core.ids)
    --taskunit.init(config.core.units)

    if not taskid.add(envname, id, 0) then
        -- don't use common.die_atomic() cuz it'll delete existing task ID.
        common.die(1, "task ID already exists\n", id)
    end
    if not taskunit.add(envname, id, tasktype, prio) then
        common.die_atomic(id, "could not create new task unit\n", id)
    end
    if not struct.create(common.genname(envname, id)) then
        common.die_atomic(id, "could not create new task structure\n", id)
    end
    --[[
    if not git.branch_create(id) then
        common.die_atomic(id, "could not create new task branch\n", id)
    end
    ]]
    return 0
end

return builtin_add
