local taskid = require("core.taskid")
local common = require("core.common")
local help = require("core.help")
local getopt = require("posix.unistd").getopt
local config = require("core.config")

--- List all task IDs.
-- Default: show only active task IDs.
local function builtin_list()
    local cmdname = "list"
    local active = true
    local completed = false
    local optstring = "Aaech"
    local keyhelp
    local envname = "work"

    for optopt, _, optind in getopt(arg, optstring) do
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
        elseif optopt == "h" then
            keyhelp = true
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 0
    end

    taskid.init(config.core.ids)
    taskid.list(envname, active, completed)
    return 0
end

return builtin_list
