local taskenv = require("core.env")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local utils = require("aux.utils")
local getopt = require("posix.unistd").getopt
local help = require("aux.help")

--- Delete task.
local function builtin_del()
    local id = arg[1]
    local envname
    local cmdname = "del"
    local unit_dir = core.struct.units.path
    local task_dir = core.struct.tasks.path
    local optstr = "fh"
    local keyhelp
    local force = false
    local last_index = 1

    for optopt, _, optind in getopt(arg, optstr) do
        if optopt == "?" then
            return core.die(1, "unrecognized option", arg[optind - 1])
        end

        last_index = optind
        if optopt == "f" then
            force = true
        elseif optopt == "h" then
            keyhelp = true
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 1
    end

    -- system dependant (fatal): load core modules
    if not taskenv.init(core.struct.envs.path) then
        return core.die(1, "could not init module taskenv", "fatal")
    elseif not taskid.init(core.struct.ids.path, core.struct.curr.path) then
        return core.die(1, "could not init module taskid", "fatal")
    elseif not taskunit.init(unit_dir, task_dir) then
        return core.die(1, "could not init module taskunit", "fatal")
    end

    -- check: user dependant: sentinel guards
    envname = taskenv.getcurr()
    if not envname then
        return core.die(1, "no current environment", "env")
    elseif not taskenv.check(envname) then
        return core.die(1, "illegal environment name value", envname)
    elseif not taskenv.exist(envname) then
        return core.die(1, "no such environment", envname)
    end

    id = arg[last_index] or taskid.getcurr(envname)
    if not id then
        return core.die(1, "no current task id", "")
    elseif not taskid.check(id) then
        return core.die(1, "illegal id value", id)
    elseif not taskid.exist(envname, id) then
        return core.die(1, "no such task id", id)
    end

    if not force then
        io.write("Task: ")
        local option = ""
        taskunit.cat(envname, id, option, "desc")
        io.write("Do you want to continue? [Yes/No] ")
        local confirm = io.read("*line")
        if confirm ~= "Yes" then
            print("deletion is cancelled")
            os.exit(1)
        end
    end

    -- delete task from database.
    local taskdir = core.struct.tasks.path .. "/" .. envname .. "/" .. id
    if not taskid.del(envname, id) then
        return core.die(1, "fatal: could not delete task id", "taskid")
    elseif not taskunit.del(envname, id) then
        return core.die(1, "fatal: could not delete task unit", "taskunit")
    elseif not utils.rm(taskdir) then
        return core.die(1, "fatal: could not delete task dir", "dir")
    end

    return 0
end

return builtin_del
