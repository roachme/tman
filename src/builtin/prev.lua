local env = require("core.env")
local git = require("core.git")
local taskid = require("core.taskid")
local common = require("core.common")
local help = require("core.help")
local config = require("core.config")
local getopt = require("posix.unistd").getopt

--- Switch to previous task.
local function tman_prev()
    local keyhelp
    local prev
    local envname = env.getcurr()
    local optstr = ":h" -- roachme:API: should option be used?
    local cmdname = "prev"

    for optopt, _, optind in getopt(arg, optstr) do
        if optopt == "?" then
            common.die(1, "unrecognized option\n", arg[optind - 1])
        end

        if optopt == "h" then
            keyhelp = true
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 0
    end

    taskid.init(config.core.ids)
    prev = taskid.getprev(envname)

    if not prev then
        common.die(1, "no previous task\n", "")
    end
    if not git.check(prev) then
        common.die(1, "errors in repo. Put meaningful desc here\n", "REPONAME")
    end

    git.branch_switch(prev)
    taskid.swap(envname)
    return 0
end

return tman_prev
