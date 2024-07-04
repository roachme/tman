local env = require("core.env")
local git = require("secondary.git")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local common = require("core.common")
local config = require("secondary.config")

--- Switch to task.
local function tman_use()
    local envname = env.getcurr()
    local id = arg[1]
    -- roachme: can't use help option cuz tman.sh fails.

    if not envname then
        return
    end

    if not envname then
        return common.die(1, "no current env\n", "env")
    end

    taskid.init(config.core.ids)

    if not id then
        common.die(1, "task ID required\n", "")
    elseif not taskid.exists(envname, id) then
        common.die(1, "task ID doesn't exist\n", id)
    end

    -- it's the same id. Do nothing.
    if taskid.getcurr(envname) == id then
        return 0
    end

    if not git.check(id) then
        common.die(1, "one of the repos has uncommited changes", "REPONAME")
    end


    local dir = config.aux.code
    local branch = taskunit.get(envname, id, "branch")
    if not branch then
        return common.die(1, "task unit file missing branch\n", id)
    end
    for _, repo in pairs(config.user.repos) do
        git.branch_switch(dir, repo.name, branch)
    end

    taskid.setcurr(envname, id)
    return 0
end

return tman_use
