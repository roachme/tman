--- Module to manipulate env names.
-- @module envdb

local envdb = {}


--[[
unitdb.lua:
    get: function
    set: function
    init: function
    save: function
]]

local efile
local envs = {}
local eregex = "(.*) (.*): (.*)"
local efmt = "%d %s: %s\n"

---Load env database.
---@return boolean
local function load()
    envs = {}
    local f = io.open(efile)

    if not f then
        return false
    end
    for line in f:lines() do
        local stat, name, desc = string.match(line, eregex)
        table.insert(
            envs,
            { status = tonumber(stat), name = name, desc = desc }
        )
    end
    return f:close() == true
end

---Save env database.
---@return boolean
local function save()
    local f = io.open(efile, "w")

    if not f then
        return false
    end

    for _, item in pairs(envs) do
        f:write(efmt:format(item.status, item.name, item.desc))
    end
    return f:close() == true
end

---Init env database.
---@param fname string
---@return boolean
function envdb.init(fname)
    efile = fname
    if not efile or efile == "" then
        return false
    end
    return load()
end

---Get env database size.
---@return number
function envdb.size()
    local size = 0

    for _, _ in pairs(envs) do
        size = size + 1
    end
    return size
end

---Get env unit by index.
---@param idx number
---@return table
function envdb.getidx(idx)
    local item = envs[idx] or {}
    return { status = item.status, name = item.name, desc = item.desc }
end

---Get environment unit values.
---@param name string
---@return table
function envdb.get(name)
    for _, item in pairs(envs) do
        if name and item.name == name then
            return { status = item.status, name = item.name, desc = item.desc }
        end
    end
    return {}
end

---Check that key exists in database.
---@param key string
---@return boolean
function envdb.ext(key)
    return next(envdb.get(key))
end

---Set environment unit value.
---@param name string
---@param key string
---@param val string
---@return boolean
function envdb.set(name, key, val)
    if not envdb.ext(name) then
        return false
    end

    if key == "name" then
        envs[val] = envs[name]
        envs[name] = nil
    elseif key == "status" then
        envs[name].status = val
    elseif key == "desc" then
        envs[name].desc = val
    end
    return true
end

---Add new env entry.
---@param name string
---@param desc string
---@return boolean
function envdb.add(name, desc, status)
    if envdb.ext(name) then
        return false
    end
    table.insert(envs, { name = name, desc = desc, status = status })
    return save()
end

---Get list of environments.
---@return table
function envdb.list()
    local list = {}

    for _, item in pairs(envs) do
        local unit = { name = item.name, desc = item.desc, status = item.status }
        table.insert(list, unit)
    end
    return list
end

---Delete an env entry.
---@param name string
---@return boolean
function envdb.del(name)
    for i, item in pairs(envs) do
        if item.name == name then
            table.remove(envs, i)
            return save()
        end
    end
    return false
end

return envdb
