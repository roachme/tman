--- Auxilary module for taskunit.lua
-- @module unit

local unit = {}
local unitdb = {}
local unitfile = ""
local unitregex = "(%w*): (.*)"
local unitfmt = "%s: %s\n"

local levels = {
    SYS = 0,
    BASIC = 1,
    ADDIT = 2,
}

unit.keys = {
    { val = "id", lvl = 1 },
    { val = "prio", lvl = 1 },
    { val = "type", lvl = 1 },
    { val = "desc", lvl = 1 },
    { val = "env", lvl = 2 },
    { val = "date", lvl = 2 },
    { val = "uniqid", lvl = 3 }, -- uniq ID for each task id (use time).
}
unit.prios = {
    highest = "highest",
    high = "high",
    mid = "mid",
    low = "low",
    lowest = "lowest",
}

---Load task units from the file.
---@return boolean
local function load_units()
    unitdb = {}
    local f = io.open(unitfile)

    if not f then
        return false
    end

    for line in f:lines() do
        local key, val = string.match(line, unitregex)
        unitdb[key] = val
    end
    return f:close() == true
end

---Init task unit database.
---@param fname string
---@return boolean
function unit.init(fname)
    unitfile = fname
    return load_units()
end

---Save task units into the file.
---@return boolean
function unit.save()
    local f = io.open(unitfile, "w")

    if not f then
        return false
    end

    for key, val in pairs(unitdb) do
        f:write(unitfmt:format(key, val))
    end
    return f:close() == true
end

----Get unit key.
---@param key string
---@return string
function unit.get(key)
    return unitdb[key]
end

---Set unit key.
---@param key string
---@param val string
---@return boolean
function unit.set(key, val)
    unitdb[key] = val
    return true
end

function unit.units()
    return unitdb
end

return unit
