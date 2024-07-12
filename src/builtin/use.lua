local env = require("core.env")
local git = require("secondary.git")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local config = require("secondary.config")
local shell = require("aux.shell")
local utils = require("aux.utils")

--- Switch to task.
local function tman_use()
    local path = config.aux.code
    local envname = env.getcurr()
    local id = arg[1]
    -- roachme: can't use help option cuz tman.sh fails.

    if not envname then
        return
    end

    if not envname then
        return core.die(1, "no current env", "env")
    end

    taskid.init(config.core.refs.ids)

    if not id then
        core.die(1, "task ID required", "")
    elseif not taskid.exists(envname, id) then
        core.die(1, "task ID doesn't exist", id)
    end

    -- it's the same id. Do nothing.
    if taskid.getcurr(envname) == id then
        return 0
    end

    -- check that repos are ready to create task branch.
    for _, repo in pairs(config.user.repos) do
        if git.repo_isuncommited(repo.name, path) then
            return core.die(1, "repo has uncommited changes", repo.name)
        end
    end

    local branch = taskunit.get(envname, id, "branch")
    if not branch then
        return core.die(1, "task unit file missing branch", id)
    end
    for _, repo in pairs(config.user.repos) do
        git.branch_switch(repo.name, branch, path)
    end

    taskid.setcurr(envname, id)
    shell.setcurr(utils.genname(envname, id))
    return 0
end

return tman_use
