local env = require("core.env")
local git = require("secondary.git")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local help = require("secondary.help")
local config = require("secondary.config")
local getopt = require("posix.unistd").getopt
local shell = require("aux.shell")
local utils = require("aux.utils")

--- Switch to previous task.
local function tman_prev()
    local keyhelp
    local prev
    local uconfig
    local path = core.struct.code.path
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

    uconfig = config.uget(envname)
    taskid.init(core.struct.ids.path)
    prev = taskid.getprev(envname)

    if not prev then
        return core.die(1, "no previous task", "prev")
    end

    local branch = taskunit.get(envname, prev, "branch")
    for _, repo in pairs(uconfig.repos) do
        if git.repo_isuncommited(repo.name, path) then
            return core.die(1, "repo has uncommited changes", repo.name)
        end
    end
    for _, repo in pairs(uconfig.repos) do
        if not git.branch_switch(repo.name, branch, path) then
            return core.die(1, "could not switch to task branch", repo.name)
        end
    end

    taskid.swap(envname)
    shell.setcurr(utils.genname(envname, prev))
    return 0
end

return tman_prev
