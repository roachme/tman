local core = require("core.core")

local shell = {}

local function getval(fname)
    local res
    local f = io.open(fname)

    if not f then
        return nil
    end
    res = f:read("*a")
    res = string.match(res, "/(.*)")
    f:close()
    return res
end

local function setval(fname, val)
    local f = io.open(fname, "w")

    if not f then
        return false
    end
    f:write(val)
    f:close()
    return true
end

---Get current task id to use from shell.
---@return string | nil
function shell.getcurr()
    local fname = core.struct.curr.path
    return getval(fname)
end

---Set current task id to use from shell.
---@param val string
---@return boolean
function shell.setcurr(val)
    local fname = core.struct.curr.path
    return setval(fname, val)
end

return shell
