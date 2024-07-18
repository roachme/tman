local taskenv = require("core.env")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local help = require("aux.help")
local getopt = require("posix.unistd").getopt
local utils = require("aux.utils")

--- Add a new task.
local function builtin_add()
    local id
    local cmdname = "add"
    local prio = "mid"
    local tasktype = "bugfix"
    -- roachme: move `-d' option (description) from taskunit.lua to here.
    local optstr = "hp:t:"
    local last_index = 1
    local keyhelp
    local envname
    -- roach: make tman accept description from command line, not CLI
    local desc = utils.get_input("Desc")
    local unit_dir = core.struct.units.path
    local task_dir = core.struct.tasks.path

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            return core.die(1, "unrecognized option", arg[optind - 1])
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

    id = arg[last_index]
    envname = arg[last_index + 1]

    if keyhelp then
        help.usage(cmdname)
        return 0
    end

    -- system dependant (fatal): load core modules
    if not taskenv.init(core.struct.envs.path) then
        return core.die(1, "could not init module taskenv", "fatal")
    elseif not taskid.init(core.struct.ids.path) then
        return core.die(1, "could not init module taskid", "fatal")
    elseif not taskunit.init(unit_dir, task_dir) then
        return core.die(1, "could not init module taskunit", "fatal")
    end

    -- check: user dependant: sentinel guards
    envname = envname or taskenv.getcurr()
    if not envname then
        return core.die(1, "no current env", "envname")
    elseif not taskenv.check(envname) then
        return core.die(1, "illegal environment name value", envname)
    end

    if not id then
        return core.die(1, "task id required", "id")
    elseif not taskenv.check(envname) then
        return core.die(1, "illegal environment name value", envname)
    elseif not taskenv.exist(envname) then
        return core.die(1, "no such environment name", envname)
    elseif not taskid.check(id) then
        return core.die(1, "illegal task id value", id)
    elseif taskid.exist(envname, id) then
        return core.die(1, "task id already exists", id)
    elseif not taskunit.check("desc", desc) then
        return core.die(1, "illegal description value", desc)
    end

    -- create all necessary stuff for new task.
    local taskdir = core.struct.tasks.path .. "/" .. envname .. "/" .. id
    if not taskid.add(envname, id) then
        return core.die(1, "fatal never: should never happen", id)
    elseif not taskunit.add(envname, id, desc, tasktype, prio) then
        taskid.del(envname, id)
        return core.die(1, "fatal never: could not create unit file", id)
    elseif not utils.mkdir(taskdir) then
        taskid.del(envname, id)
        taskunit.del(envname, id)
        return core.die(1, "fatal never: could not create task dir", "taskdir")
    end

    return 0
end

return builtin_add
