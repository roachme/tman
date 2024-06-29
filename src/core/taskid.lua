--- Operate on task units in database.
-- Like add, delete, list task IDs and so on.
-- @module TaskID

local ids = require("aux.iddb")
local shell = require("aux.shell")
local taskunit = require("core.taskunit")
local config = require("core.config")

local taskid = {}


shell.init(config.core.base)


-- Hold special task IDs: curr and prev. To minimize lookup.
local curr, prev

--- Types of task IDs.
local status = {
    CURR = 0, -- current task
    PREV = 1, -- previous task
    ACTV = 2, -- active task
    COMP = 3, -- complete task
}

-- Private functions: start --

--- Unset previous task ID.
-- Assumes that ID exists in database.
-- @param taskstatus task status to move a previous ID to
-- @return true on success, otherwise false
local function unsetprev(taskstatus)
    -- roachme: use `ids.get()' to unset specific ID and optimize code.
    local size = ids.size()
    taskstatus = taskstatus or status.ACTV

    for i = 1, size do
        local entry = ids.getidx(i)
        if entry.status == status.PREV then
            return ids.set(entry.id, taskstatus)
        end
    end
    return false
end

--- Set previous task ID.
-- Unset old previous task ID.
-- @param id task ID
-- @treturn bool true if previous task is set, otherwise false
local function setprev(id)
    unsetprev()
    prev = id
    return ids.set(id, status.PREV)
end

--- Unset current task ID.
-- Assumes that ID exists in database.
-- @param taskstatus task status to move a current ID to
-- @return true on success, otherwise false
local function unsetcurr(taskstatus)
    -- roachme: use `ids.get()' to unset specific ID and optimize code.
    local size = ids.size()
    taskstatus = taskstatus or status.ACTV

    for i = 1, size do
        local entry = ids.getidx(i)
        if entry.status == status.CURR then
            return ids.set(entry.id, taskstatus)
        end
    end
    return false
end

--- Set current task ID.
-- @param id task ID
-- @treturn bool true if previous task is set, otherwise false
local function setcurr(id)
    unsetcurr()
    curr = id
    return ids.set(id, status.CURR)
end

--- Load curr & prev IDs into local variables to minimize lookup.
local function _load_special_ids()
    local size = ids.size()

    for i = 1, size do
        local entry = ids.getidx(i)

        if entry.status == status.CURR then
            curr = entry.id
        elseif entry.status == status.PREV then
            prev = entry.id
        end
    end
end

-- Private functions: end --

-- Public functions: start --

--- Check that task ID exist.
-- @param id task ID to look up
-- @treturn bool true if task ID exist, otherwise false
function taskid.exist(id)
    return ids.exist(id)
end

--- Get previous task ID from database.
-- @return previous task ID
-- @return on success - previous task ID
-- @return on failure - nil
function taskid.getprev()
    return prev
end

--- Get current task ID from database.
-- @return current task ID
-- @return on success - current task ID
-- @return on failure - nil
function taskid.getcurr()
    return curr
end

--- Swap current and previous task IDs.
function taskid.swap()
    local tmpprev = taskid.getprev()

    setprev(curr)
    setcurr(tmpprev)
    return ids.save()
end

--- Add a new task ID.
-- @param id task ID to add to database
-- @treturn bool true on success, otherwise false
function taskid.add(id)
    -- roacme: Don't make it current.
    --         Add it to database with status: ACTV
    --         There's setcurr() for it.
    if ids.add(id, status.CURR) == false then
        return false
    end

    setprev(curr)
    setcurr(id)

    -- set stuff for shell part of the util.
    shell.setcurr(id)

    return ids.save()
end

--- Delete a task ID.
-- @param id task ID
-- @treturn bool true on success, otherwise false
function taskid.del(id)
    if not taskid.exist(id) then
        return false
    end

    ids.del(id)

    -- update special IDs.
    if id == curr then
        return taskid.swap()
    end

    return ids.save()
end

--- Move task ID to new status.
-- roachme: Under development.
-- @param id task ID
-- @param taskstatus task new status (default: active)
-- @return true on success, otherwise false
function taskid.move(id, taskstatus)
    if id == curr then
        unsetprev(status.ACTV)
        ids.set(prev, status.CURR)
        ids.set(curr, taskstatus)
    elseif id == prev then
        ids.set(prev, status.COMP)
    else
        ids.set(id, taskstatus)
    end
    return ids.save()
end

--- Move current task to completed status.
function taskid.unsetcurr()
    -- roachme: maybe it should be ACTV?
    unsetcurr(status.COMP)
    return taskid.swap()
end

--- Set task ID as current.
-- Set previous task ID if needed.
function taskid.setcurr(id)
    -- don't do unnecessary work.
    if not id or id == curr then
        return false
    end

    setprev(curr)
    setcurr(id)
    return ids.save()
end

--- List task IDs.
-- There are 4 statuses: current, previous, active and completed. Default: active
-- @param active list only active task IDs
-- @param completed list only completed task IDs
-- @return count of task IDs
function taskid.list(active, completed)
    local desc
    local size = ids.size()

    if active and curr then
        desc = taskunit.get(curr, "desc")
        print(("* %-10s %s"):format(curr, desc))
    end
    if active and prev then
        desc = taskunit.get(prev, "desc")
        print(("- %-10s %s"):format(prev, desc))
    end

    for idx = 1, size do
        local entry = ids.getidx(idx)
        if entry.id ~= curr and entry.id ~= prev then
            if entry.status == status.ACTV and active then
                desc = taskunit.get(entry.id, "desc")
                print(("  %-10s %s"):format(entry.id, desc))
            elseif entry.status == status.COMP and completed then
                desc = taskunit.get(entry.id, "desc")
                print(("  %-10s %s"):format(entry.id, desc))
            end
        end
    end
    return size
end

-- Public functions: end --

ids.init(config.core.ids)
_load_special_ids()

return taskid
