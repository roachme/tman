--- Operate on task ids in database.
-- Like add, delete, list task ids and so on.
-- @module taskid

local ids = require("aux.iddb")

---@alias Status
---| 0   # Current task
---| 1   # Previous task
---| 2   # Active task
---| 3   # Completed task

local taskid = {}
local fids

---Types of task ids.
taskid.status = {
    CURR = 0,
    PREV = 1,
    ACTV = 2,
    COMP = 3,
}

---Check task id exists in specified environment.
---@param envname string
---@param id string
---@return boolean
function taskid.exists(envname, id)
    return ids.exist(envname, id)
end

---Get environment's current task id.
---@param envname string
---@return string|nil
function taskid.getcurr(envname)
    for i = 1, ids.size() do
        local item = ids.getidx(envname, i)
        if item.envname == envname and item.status == taskid.status.CURR then
            return item.id
        end
    end
    return nil
end

---Get environment's previous task id.
---@param envname string
---@return string|nil
function taskid.getprev(envname)
    for i = 1, ids.size() do
        local item = ids.getidx(envname, i)
        if item.envname == envname and item.status == taskid.status.PREV then
            return item.id
        end
    end
    return nil
end

---Unset environment's current task id.
---@param envname string
---@return boolean
function taskid.unsetcurr(envname)
    local curr = taskid.getcurr(envname)

    if not curr then
        return false
    end

    ids.set(envname, curr, taskid.status.ACTV)
    ids.save()
    return true
end

---Set environment's current task id.
---Also update previous one.
---@param envname string
---@param id string
---@return boolean
function taskid.setcurr(envname, id)
    local prev = taskid.getprev(envname)
    local curr = taskid.getcurr(envname)

    if not ids.exist(envname, id) then
        return false
    end
    -- sentinel guard: don't mark the same id as current
    if id == curr then
        return true
    end

    -- if theree's previous task then unmark it
    if prev then
        ids.set(envname, prev, taskid.status.ACTV)
    end

    if curr then
        ids.set(envname, curr, taskid.status.PREV)
    end
    ids.set(envname, id, taskid.status.CURR)
    ids.save()
    return true
end

---Swap current and previous task ids.
---Only if both current and previous exist.
---@param envname string
function taskid.swap(envname)
    local newcurr = taskid.getprev(envname)

    if newcurr then
        return taskid.setcurr(envname, newcurr)
    end
    return false
end

---Rename task id.
---@param oldid string
---@param newid string
---@return boolean
function taskid.rename(envname, oldid, newid)
    if not ids.exist(envname, oldid) then
        return false
    end

    local item = ids.get(envname, oldid)
    ids.del(envname, oldid)
    ids.add(envname, newid, item.status)
    return ids.save()
end

---Add new current task id into environment.
---@param envname string
---@param id string
---@return boolean
function taskid.add(envname, id)
    if not ids.add(envname, id, taskid.status.ACTV) then
        return false
    end
    return taskid.setcurr(envname, id)
end

---Delete a task id from environment.
---@param envname string
---@param id string
---@return boolean
function taskid.del(envname, id)
    local prev = taskid.getprev(envname)
    local curr = taskid.getcurr(envname)

    if not ids.del(envname, id) then
        return false
    end

    if id == curr and prev then
        taskid.setcurr(envname, prev)
    end
    return ids.save()
end

---Init task id database.
---@param fname string
---@return boolean
function taskid.init(fname)
    fids = fname
    return ids.init(fids)
end

---Get task id by index.
---@param envname string
---@param idx number
---@return table
function taskid.getidx(envname, idx)
    return ids.getidx(envname, idx)
end

---Get number of task ids.
---@return number
function taskid.size()
    return ids.size()
end

return taskid
