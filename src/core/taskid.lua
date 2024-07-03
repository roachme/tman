--- Operate on task ids in database.
-- Like add, delete, list task IDs and so on.
-- @module taskid

local ids = require("aux.iddb")
local shell = require("aux.shell")
local taskunit = require("core.taskunit")

---@alias Status
---| 0   # Current task
---| 1   # Previous task
---| 2   # Active task
---| 3   # Completed task

local taskid = {}
local fids

--- Types of task IDs.
local status = {
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
        if item.envname == envname and item.status == status.CURR then
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
        if item.envname == envname and item.status == status.PREV then
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

    ids.set(envname, curr, status.ACTV)
    ids.save()
    shell.setcurr("")
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
        ids.set(envname, prev, status.ACTV)
    end

    if curr then
        ids.set(envname, curr, status.PREV)
    end
    ids.set(envname, id, status.CURR)
    ids.save()
    shell.setcurr(id)
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

---Add new current task id into environment.
---@param envname string
---@param id string
---@param stat Status
---@return boolean
function taskid.add(envname, id, stat)
    -- roachme: not used...
    stat = stat or status.CURR

    if not ids.add(envname, id, status.ACTV) then
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

    if not taskid.exists(envname, id) then
        return false
    end

    ids.del(envname, id)

    -- if current task id gets deleted then mark previous task id as current
    if id == curr then
        taskid.unsetcurr(envname)
    elseif id == curr and prev then
        taskid.setcurr(envname, prev)
    end

    return ids.save()
end

function taskid.init(fname)
    fids = fname
    ids.init(fids)
end

---List environment's task ids.
---@param envname string
function taskid.list(envname, active, completed)
    local desc
    local prev = taskid.getprev(envname)
    local curr = taskid.getcurr(envname)

    if (active or (not active and not completed)) and curr then
        desc = taskunit.get(envname, curr, "desc")
        print(("* %-10s %s"):format(curr, desc))
    end
    if (active or (not active and not completed)) and prev then
        desc = taskunit.get(envname, prev, "desc")
        print(("- %-10s %s"):format(curr, desc))
    end

    for i = 1, ids.size() do
        local item = ids.getidx(envname, i)
        desc = taskunit.get(envname, item.id, "desc")

        if item.id ~= curr and item.id ~= prev then
            if item.status == status.ACTV and active then
                print(("a %-10s %s"):format(curr, desc))
            elseif item.status == status.COMP and completed then
                print(("c %-10s %s"):format(curr, desc))
            end
        end
    end
end

return taskid
