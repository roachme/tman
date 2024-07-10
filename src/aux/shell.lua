local config = require("secondary.config")

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
    local fname = config.core.path .. "curr"
    return getval(fname)
end

---Set current task id to use from shell.
---@param val string
---@return boolean
function shell.setcurr(val)
    local fname = config.core.path .. "curr"
    return setval(fname, val)
end

---Get current environment to use from shell.
---@return string | nil
function shell.getenv()
    local fname = config.core.path .. "env"
    return getval(fname)
end

---Get current environment to use from shell.
---@return boolean
function shell.setenv(val)
    local fname = config.core.path .. "env"
    return setval(fname, val)
end

return shell
