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
    local active = false
    local completed = false
    local optstring = "Aace:h"
    local keyhelp
    local envname = env.getcurr()

    for optopt, optarg, optind in getopt(arg, optstring) do
        if optopt == "?" then
            common.die(1, "unrecognized option\n", arg[optind - 1])
        end

        if optopt == "A" then
            active = true
            completed = true
        elseif optopt == "a" then
            active = true
            completed = false
        elseif optopt == "c" then
            active = false
            completed = true
        elseif optopt == "e" then
            envname = optarg
        elseif optopt == "h" then
            keyhelp = true
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 0
    end
    if not envname then
        return
    end

    -- check that envname exists.
    if not env.exists(envname) then
        common.die(1, "no such environment name\n", envname)
    end

    taskid.init(config.core.ids)
    taskid.list(envname, active, completed)
    return 0
end

return builtin_list
