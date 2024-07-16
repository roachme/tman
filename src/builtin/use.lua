local env = require("core.env")
local taskid = require("core.taskid")
local core = require("core.core")
local shell = require("core.shell")
local utils = require("aux.utils")

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
    shell.setcurr(utils.genname(envname, id))
    return 0
end

return tman_use
