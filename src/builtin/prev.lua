local env = require("core.env")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local help = require("aux.help")
local getopt = require("posix.unistd").getopt
local plugin = require("core.plugin")

--- Switch to previous task.
local function tman_prev()
    local keyhelp
    local prev
    local envname = env.getcurr()
    local optstr = ":h" -- roachme:API: should option be used?
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

    if not envname then
        return core.die(1, "no current env", "env")
    elseif not env.exists(envname) then
        return core.die(1, "no such env", "env")
    end

    taskid.init(core.struct.ids.path)
    prev = taskid.getprev(envname)

    if not prev then
        return core.die(1, "no previous task", "prev")
    end

    taskid.swap(envname)

    if not plugin.init(envname) then
        return core.die(1, "could not init plugin", "plugin")
    end

    local branch = taskunit.get(envname, prev, "branch")
    plugin.git.switch(prev, branch)
    return 0
end

return tman_prev
