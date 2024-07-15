local env = require("core.env")
local gitlib = require("aux.gitlib")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local config = require("struct.config")
local shell = require("core.shell")
local utils = require("aux.utils")

--- Switch to task.
local function tman_use()
    local uconfig
    local path = core.struct.code.path
    local envname = env.getcurr()
    local id = arg[1]
    -- roachme: can't use help option cuz tman.sh fails.

    if not envname then
        return
    end

    if not envname then
        return core.die(1, "no current env", "env")
    end

    uconfig = config.uget(envname)
    taskid.init(core.struct.ids.path)

    if not id then
        core.die(1, "task id required", "")
    elseif not taskid.exists(envname, id) then
        core.die(1, "task id doesn't exist", id)
    end

    -- it's the same id. Do nothing.
    if taskid.getcurr(envname) == id then
        return 0
    end

    -- check that repos are ready to create task branch.
    for _, repo in pairs(uconfig.repos) do
        if gitlib.repo_isuncommited(repo.name, path) then
            return core.die(1, "repo has uncommited changes", repo.name)
        end
    end

    local branch = taskunit.get(envname, id, "branch")
    if not branch then
        return core.die(1, "task unit file missing branch", id)
    end
    for _, repo in pairs(uconfig.repos) do
        gitlib.branch_switch(repo.name, branch, path)
    end

    taskid.setcurr(envname, id)
    shell.setcurr(utils.genname(envname, id))
    return 0
end

return tman_use
