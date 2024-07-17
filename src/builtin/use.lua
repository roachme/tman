local env = require("core.env")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local plugin = require("core.plugin")

--- Switch to task.
local function tman_use()
    local envname = env.getcurr()
    local id = arg[1]
    -- roachme: can't use help option cuz tman.sh fails.

    if not envname then
        return
    end

    if not envname then
        return core.die(1, "no current env", "env")
    end

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

    taskid.setcurr(envname, id)

    if not plugin.init(envname, id) then
        return core.die(1, "could not init plugin", "plugin")
    end

    local branch = taskunit.get(envname, id, "branch")
    plugin.git.switch(branch)
    return 0
end

return tman_use
