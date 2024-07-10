local env = require("core.env")
local git = require("secondary.git")
local taskid = require("core.taskid")
local struct = require("secondary.struct")
local taskunit = require("core.taskunit")
local common = require("core.common")
local help = require("secondary.help")
local getopt = require("posix.unistd").getopt
local config = require("secondary.config")

--- Add a new task.
-- Fill the rest with default values.
-- @see tman_set
-- @return on success - true
-- @return on failrue - false
local function builtin_add()
    local id
    local cmdname = "add"
    local prio = "mid"
    local tasktype = "bugfix"
    -- roachme: move `-d' option (description) from taskunit.lua to here.
    local optstr = "hp:t:"
    local last_index = 1
    local keyhelp
    local envname = env.getcurr()
    local path = config.aux.code

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            return common.die(1, "unrecognized option\n", arg[optind - 1])
        end

        last_index = optind
        if optopt == "h" then
            keyhelp = true
        elseif optopt == "p" then
            prio = optarg
        elseif optopt == "t" then
            tasktype = optarg
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 0
    end

    id = arg[last_index]
    taskid.init(config.core.refs.ids)

    if not envname then
        return common.die(1, "no current env\n", "envname")
    elseif not env.exists(envname) then
        return common.die(1, "no such env\n", envname)
    elseif not id then
        return common.die(1, "task id required\n", "id")
    elseif taskid.exists(envname, id) then
        return common.die(1, "task id exists\n", id)
    end

    -- check that repos are ready to create task branch.
    for _, repo in pairs(config.user.repos) do
        if git.repo_isuncommited(repo.name, path) then
            return common.die(1, "repo has uncommited changes\n", repo.name)
        end
    end

    -- create all necessary stuff for new task.
    if not taskid.add(envname, id) then
        common.die(1, "task ID already exists\n", id)
    elseif not taskunit.add(envname, id, tasktype, prio) then
        taskid.del(envname, id)
        common.die(1, "could not create new task unit\n", id)
    elseif not struct.create(envname, id) then
        taskid.del(envname, id)
        taskunit.del(envname, id)
        common.die(id, "could not create new task structure\n", id)
    end

    -- create task branch in repos.
    local branch = taskunit.get(envname, id, "branch")
    for _, repo in pairs(config.user.repos) do
        if not git.branch_create(repo.name, branch, path) then
            taskid.del(envname, id)
            taskunit.del(envname, id)
            struct.delete(id)
            common.die(1, "could not create new task branch\n", id)
        end
        git.branch_switch(repo.name, branch, path)
    end
    return 0
end

return builtin_add
