local env = require("core.env")
local config = require("secondary.config")
local taskid = require("core.taskid")
local core = require("core.core")

--- Get tman items.
-- Like prev/ curr task id, etc.
local function builtin_get()
    local item = arg[1] or "curr"
    local envname = env.getcurr()

    if not envname then
        return core.die(1, "no current env", "env")
    end

    taskid.init(core.struct.ids.path)

    if item == "curr" then
        print(taskid.getcurr(envname) or "")
        return 0
    elseif item == "prev" then
        print(taskid.getprev(envname) or "")
        return 0
    elseif item == "env" then
        print(env.getcurr() or "")
        return 0
    end

    -- error handling
    core.die(1, "no such task item", item)
end

return builtin_get
