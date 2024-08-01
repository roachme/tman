local taskenv = require("core.taskenv")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local help = require("aux.help")
local getopt = require("posix.unistd").getopt

--- Show task unit metadata.
local function builtin_cat()
    local id
    local envname
    local last_index = 1
    local optstr = "hk:"
    local key, keyhelp
    local cmdname = "cat"

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            core.die(1, "unrecognized option", arg[optind - 1])
        end
        last_index = optind
        if optopt == "k" then
            key = optarg
        elseif optopt == "h" then
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
    elseif not taskunit.init(core.struct.units.path) then
        return core.die(1, "could not init module taskunit", "fatal")
    end

    envname = arg[last_index + 1] or taskenv.getcurr()
    if not envname then
        return core.die(1, "no current environment", "cat")
    elseif not taskenv.check(envname) then
        return core.die(1, "illegal environment name value", envname)
    elseif not taskenv.exist(envname) then
        return core.die(1, "no such environment", "cat")
    end

    id = arg[last_index] or taskid.getcurr(envname)
    if not id then
        return core.die(1, "no current task id", "")
    elseif not taskid.check(id) then
        return core.die(1, "illegal task id value", id)
    elseif not taskid.exist(envname, id) then
        return core.die(1, "no such task id", id)
    end

    local option = "" -- basic, additional, system, full
    if not taskunit.cat(envname, id, option, key) then
        if key then
            core.die(1, "no such key", key)
        end
    end
    return 0
end

return builtin_cat
