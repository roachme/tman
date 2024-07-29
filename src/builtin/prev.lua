local help = require("aux.help")
local core = require("core.core")
local getopt = require("posix.unistd").getopt
local taskid = require("core.taskid")
local taskenv = require("core.env")
local taskunit = require("core.taskunit")

--- Switch to previous task.
local function tman_prev()
    local keyhelp
    local prev
    local envname
    local optstr = ":h" -- roachme:API: should option be used?
    local cmdname = "prev"
    local unit_dir = core.struct.units.path
    local task_dir = core.struct.tasks.path

    for optopt, _, optind in getopt(arg, optstr) do
        if optopt == "?" then
            core.die(1, "unrecognized option", arg[optind - 1])
        end

        if optopt == "h" then
            keyhelp = true
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 0
    end

    -- system dependant (fatal): load core modules
    if not taskenv.init(core.struct.envs.path) then
        return core.die(1, "could not init module taskenv", "fatal")
    elseif not taskid.init(core.struct.ids.path, core.struct.curr.path) then
        return core.die(1, "could not init module taskid", "fatal")
    elseif not taskunit.init(unit_dir, task_dir) then
        return core.die(1, "could not init module taskunit", "fatal")
    end

    -- sentilen guards
    envname = arg[1] or taskenv.getcurr()
    if not envname then
        return core.die(1, "no current environment", "env")
    elseif not taskenv.check(envname) then
        return core.die(1, "illegal environment name value", envname)
    elseif not taskenv.exist(envname) then
        return core.die(1, "no such environment", "env")
    end

    prev = taskid.getprev(envname)
    if not prev then
        return core.die(1, "no previous task", "prev")
    end

    taskid.swap(envname)
    return 0
end

return tman_prev
