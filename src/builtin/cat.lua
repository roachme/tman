local env = require("core.env")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local help = require("secondary.help")
local getopt = require("posix.unistd").getopt

--- Show task unit metadata.
local function builtin_cat()
    local id
    local last_index = 1
    local optstr = "hk:"
    local key, keyhelp
    local cmdname = "cat"
    local envname = env.getcurr()

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

    taskid.init(core.struct.ids.path)

    if not envname then
        return core.die(1, "no current environment", "cat")
    elseif not env.exists(envname) then
        return core.die(1, "no such environment", "cat")
    end

    id = arg[last_index] or taskid.getcurr(envname)
    if not id then
        core.die(1, "no current task id", "")
    elseif not taskid.exists(envname, id) then
        core.die(1, "no such task id", id)
    end

    if not taskunit.cat(envname, id, key) then
        if key then
            core.die(1, "no such key", key)
        end
    end
    return 0
end

return builtin_cat
