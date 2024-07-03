--- Auxilary module for taskunit.lua
-- @module unit

local unit = {}
local units = {}
local unitfile = ""
local unitregex = "(%w*): (.*)"
local unitfmt = "%s: %s\n"

unit.keys = {
    "id",
    "prio",
    "type",
    "desc",
    "time",
    "date",
    "link",
    "repos",
    "envname",
    "status",
    "branch",
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
    units = {}
    local f = io.open(unitfile)

    if not f then
        return false
    end

    for line in f:lines() do
        local key, val = string.match(line, unitregex)
        units[key] = val
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

    for key, val in pairs(units) do
        f:write(unitfmt:format(key, val))
    end
    return f:close() == true
end

----Get unit key.
---@param key string
---@return string | nil
function unit.get(key)
    return units[key]
end

---Set unit key.
---@param key string
---@param val string
---@return boolean
function unit.set(key, val)
    units[key] = val
    return true
end

function unit.check()
    -- roachme: value can be nil. Gotta find a better logic.
    return true
end

return unit
