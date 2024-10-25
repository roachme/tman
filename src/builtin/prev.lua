local core = require("core.core")
local help = require("aux.help")
local getopt = require("posix.unistd").getopt

--- Switch to previous task.
local function tman_prev()
    local keyhelp
    local optstr = ":h"
    local cmdname = "prev"

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

    core.id_prev()

    local curr = core.getcurr()
    local newdir = core.prefix .. "/tasks/" .. curr.env .. "/" .. curr.curr
    print(newdir)
    return 0
end

return tman_prev
