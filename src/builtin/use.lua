local env = require("core.env")
local git = require("core.git")
local taskid = require("core.taskid")
local common = require("core.common")
local config = require("core.config")

--- Switch to task.
local function tman_use()
    local envname = env.getcurr()
    local id = arg[1]
    -- roachme: can't use help option cuz tman.sh fails.

    if not envname  then
        return
    end

    taskid.init(config.core.ids)

    if not id then
        common.die(1, "task ID required\n", "")
    end
    if not taskid.exists(envname, id) then
        common.die(1, "task ID doesn't exist\n", id)
    end
    if taskid.getcurr(envname) == id then
        common.die(1, "already in use\n", id)
    end
    if not git.check(id) then
        common.die(1, "one of the repos has uncommited changes", "REPONAME")
    end

    git.branch_switch(id)
    taskid.setcurr(envname, id)
    return 0
end

return tman_use
