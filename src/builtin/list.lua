local env = require("core.env")
local taskid = require("core.taskid")
local common = require("core.common")
local help = require("secondary.help")
local getopt = require("posix.unistd").getopt
local config = require("secondary.config")

--- List all task IDs.
-- Default: show only active task IDs.
local function builtin_list()
    local cmdname = "list"
    local active = true
    local completed = false
    local optstring = "Aach"
    local keyhelp
    local envname
    local last_index = 1

    for optopt, _, optind in getopt(arg, optstring) do
        if optopt == "?" then
            common.die(1, "unrecognized option\n", arg[optind - 1])
        end

        last_index = optind
        if optopt == "A" then
            active = true
            completed = true
        elseif optopt == "a" then
            active = true
            completed = false
        elseif optopt == "c" then
            active = false
            completed = true
        elseif optopt == "h" then
            keyhelp = true
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 0
    end

    envname = arg[last_index] or env.getcurr()
    if not envname then
        return common.die(1, "no current env\n", "env")
    elseif not env.exists(envname) then
        return common.die(1, "no such environment name\n", envname)
    end

    taskid.init(config.core.refs.ids)
    taskid.list(envname, active, completed)
    return 0
end

return builtin_list
