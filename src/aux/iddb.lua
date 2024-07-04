--- Simple database for task IDs.
-- @module db

local db = {}
local idfile = ""
local idregex = "(%w*) ([%w-]*) (%d)"
local idfmt = "%s %s %d\n"
local ids = {}

---Load task IDs from database.
---@return boolean
local function _db_load()
    db = {} -- reset database.
    local f = io.open(idfile, "r")

    if not f then
        return false
    end

    for line in f:lines() do
        local envname, id, idstatus = string.match(line, idregex)
        table.insert(
            db,
            { envname = envname, id = id, status = tonumber(idstatus) }
        )
    end
    return f:close() and true or false
end

---Init database (load task IDs from the file).
---@param fname string
function ids.init(fname)
    idfile = fname
    return _db_load()
end

---Check that variable entry's safe to save.
---@return boolean
function ids.check()
    for _, item in pairs(db) do
        if
            type(item.id) ~= "string"
            or type(item.id) ~= "string"
            or type(item.status) ~= "number"
        then
            return false
        end
    end
    return true
end

---Check that task ID exist in database.
---@param envname string
---@param id string
---@return boolean
function ids.exist(envname, id)
    for _, item in pairs(db) do
        if item.envname == envname and item.id == id then
            return true
        end
    end
    return false
end

---Save task IDs to file.
---@return boolean
function ids.save()
    local f = io.open(idfile, "w")

    if not f then
        return false
    end
    if not ids.check() then
        return false
    end

    for _, item in pairs(db) do
        f:write(idfmt:format(item.envname, item.id, item.status))
    end
    return f:close() and true or false
end

---Add new task ID into database.
---@param envname string
---@param id string
---@param status number
---@return boolean
function ids.add(envname, id, status)
    if ids.exist(envname, id) then
        return false
    end
    table.insert(db, { envname = envname, id = id, status = status })
    return true
end

---Delete task ID from database.
---@param id string
---@return boolean
function ids.del(envname, id)
    for i, item in pairs(db) do
        if envname == item.envname and item.id == id then
            table.remove(db, i)
            return true
        end
    end
    return false
end

---Get size of database entries.
---@return number
function ids.size()
    local size = 0

    for _, _ in pairs(db) do
        size = size + 1
    end
    return size
end

---Get an item from database by index (generic).
---@param idx number
---@return table
function ids.getidx_generic(idx)
    return db[idx] or {}
end

---Get an item from database by index.
---@param idx number
---@return table
function ids.getidx(envname, idx)
    local item = db[idx] or {}

    if item.envname == envname then
        return { envname = item.envname, id = item.id, status = item.status }
    end
    return {}
end

---Set a status to task ID.
---@param id string
---@param status Status
---@return boolean
function ids.set(envname, id, status)
    for _, item in pairs(db) do
        if envname == item.envname and item.id == id then
            item.status = status
            return true
        end
    end
    return false
end

return ids
