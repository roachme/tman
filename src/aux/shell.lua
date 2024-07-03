local shell = {}

local function getval(fname)
    local res
    local f = io.open(fname)

    if not f then
        return nil
    end
    res = f:read("*a")
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

---Set current task id to use from shell.
---@param val string
function shell.setcurr(val)
    local fname = "curr"
    return setval(fname, val)
end


return shell
