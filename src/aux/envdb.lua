--- Module to manipulate env names.
-- @module envdb

local envdb = {}

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

---Check that envname exists in database.
---@param name string
---@return boolean
function envdb.exists(name)
    for _, item in pairs(envs) do
        if item.name == name then
            return true
        end
    end
    return false
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

---Get env unit.
---@param name string
---@return table
function envdb.get(name)
    for _, item in pairs(envs) do
        if item.name == name then
            return { status = item.status, name = item.name, desc = item.desc }
        end
    end
    return {}
end

---Get env unit by index.
---@param idx number
---@return table
function envdb.getidx(idx)
    local item = envs[idx] or {}
    return { status = item.status, name = item.name, desc = item.desc }
end

---Set env item's status.
---@param name string
---@param status Status
function envdb.set(name, status)
    for _, item in pairs(envs) do
        if item.name == name then
            item.status = status
            return save()
        end
    end
    return false
end

---Add new env entry.
---@param name string
---@param desc string
---@return boolean
function envdb.add(name, desc, status)
    if envdb.exists(name) then
        return false
    end
    table.insert(envs, { name = name, desc = desc, status = status })
    return save()
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
