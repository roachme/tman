local env = require("core.env")
local config = require("secondary.config")
local taskid = require("core.taskid")
local common = require("core.common")

--- Get tman items.
-- Like prev/ curr task ID, etc.
local function builtin_get()
    local item = arg[1] or "curr"
    local envname = env.getcurr()

    if not envname then
        return common.die(1, "no current env\n", "env")
    end

    taskid.init(config.core.refs.ids)

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
    common.die(1, "no such task item\n", item)
end

return builtin_get
