local env = require("core.env")
local git = require("secondary.git")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local common = require("core.common")
local help = require("secondary.help")
local config = require("secondary.config")
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

    if not envname then
        return common.die(1, "no current env\n", "env")
    elseif not env.exists(envname) then
        return common.die(1, "no such env\n", "env")
    end

    taskid.init(config.core.ids)
    prev = taskid.getprev(envname)

    if not prev then
        return common.die(1, "no previous task\n", "prev")
    end

    local path = config.aux.code
    local branch = taskunit.get(envname, prev, "branch")
    if not branch then
        return common.die_atomic(prev, "task unit file missing branch\n", prev)
    end

    for _, repo in pairs(config.user.repos) do
        if not git.repo_isuncommited(repo.name, path) then
            return common.die(1, "repo has uncommited changes\n", repo.name)
        end
    end
    for _, repo in pairs(config.user.repos) do
        if not git.branch_switch(repo.name, branch, path) then
            common.die(prev, "could not create new task branch\n", prev)
        end
        git.branch_switch(repo.name, branch, path)
    end

    taskid.swap(envname)
    return 0
end

return tman_prev
