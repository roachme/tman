local env = require("core.env")
local git = require("secondary.git")
local taskid = require("core.taskid")
local struct = require("secondary.struct")
local taskunit = require("core.taskunit")
local core = require("core.core")
local help = require("secondary.help")
local getopt = require("posix.unistd").getopt
local config = require("secondary.config")
local shell = require("aux.shell")
local utils = require("aux.utils")

--- Add a new task.
-- Fill the rest with default values.
-- @see tman_set
-- @return on success - true
-- @return on failrue - false
local function builtin_add()
    local id
    local uconfig
    local cmdname = "add"
    local prio = "mid"
    local tasktype = "bugfix"
    -- roachme: move `-d' option (description) from taskunit.lua to here.
    local optstr = "hp:t:"
    local last_index = 1
    local keyhelp
    local envname = env.getcurr()
    local path = core.struct.code.path

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            return core.die(1, "unrecognized option", arg[optind - 1])
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
    taskid.init(core.struct.ids.path)

    if not envname then
        return core.die(1, "no current env", "envname")
    elseif not env.exists(envname) then
        return core.die(1, "no such env", envname)
    elseif not id then
        return core.die(1, "task id required", "id")
    elseif taskid.exists(envname, id) then
        return core.die(1, "task id exists", id)
    end

    uconfig = config.uget(envname)
    -- check that repos are ready to create task branch.
    for _, repo in pairs(uconfig.repos) do
        if git.repo_isuncommited(repo.name, path) then
            return core.die(1, "repo has uncommited changes", repo.name)
        end
    end

    -- read task description.
    local desc = utils.get_input("Desc")

    -- create all necessary stuff for new task.
    if not taskid.add(envname, id) then
        core.die(1, "task id already exists", id)
    elseif not taskunit.add(envname, id, desc, tasktype, prio) then
        taskid.del(envname, id)
        core.die(1, "could not create new task unit", id)
    elseif not struct.create(envname, id) then
        taskid.del(envname, id)
        taskunit.del(envname, id)
        core.die(id, "could not create new task structure", id)
    end

    -- create task branch in repos.
    local branch = taskunit.get(envname, id, "branch")
    for _, repo in pairs(uconfig.repos) do
        if not git.branch_create(repo.name, branch, path) then
            taskid.del(envname, id)
            taskunit.del(envname, id)
            struct.delete(id)
            core.die(1, "could not create new task branch", id)
        end
        git.branch_switch(repo.name, branch, path)
    end

    -- cache current task id
    shell.setcurr(utils.genname(envname, id))

    return 0
end

return builtin_add
