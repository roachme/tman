--- Operate on task unit inside meta data.
-- Metadata like date, description and so on.
-- @module TaskUnit

local utils = require("aux.utils")
local unit = require("aux.unitdb")

local taskunit = {}

local unit_dir, task_dir

---Check task id.
---@param id string
---@return boolean
local function _check_id(id)
    local descregex = "[a-zA-Z0-9_%s-]*"
    if string.match(id, descregex) == id then
        return true
    end
    return false
end

---Check description.
---@param desc string
---@return boolean
local function _check_desc(desc)
    local descregex = "[a-zA-Z0-9_%s-]*"
    if string.match(desc, descregex) == desc then
        return true
    end
    return false
end

---Check priority.
---@param priority string
---@return boolean
local function _check_prio(priority)
    for _, prio in pairs(unit.prios) do
        if prio == priority then
            return true
        end
    end
    return false
end

---Check task type.
---@param type string
---@return boolean
local function _check_type(type)
    local tasktypes = { "bugfix", "feature", "hotfix" }

    for _, dtype in pairs(tasktypes) do
        if type == dtype then
            return true
        end
    end
    return false
end

---Amend task description.
---@param envname string
---@param id string
---@param newdesc string
---@return boolean
local function _set_desc(envname, id, newdesc)
    unit.init(unit_dir .. utils.genname(envname, id))
    unit.set("desc", newdesc)
    return unit.save()
end

---Change task id.
---@param id string
---@param newid string
---@return boolean
local function _set_id(envname, id, newid)
    -- rocahme: outta use struct.lua
    local old_taskdir = task_dir .. utils.genname(envname, id)
    local new_taskdir = task_dir .. utils.genname(envname, newid)

    unit.init(unit_dir .. utils.genname(envname, id))
    unit.set("id", newid)
    unit.save()

    -- gotta update curr & prev task ids.
    return utils.rename(old_taskdir, new_taskdir)
end

---Change task type.
---@return boolean
local function _set_type(envname, id, newtype)
    unit.init(unit_dir .. utils.genname(envname, id))
    unit.set("type", newtype)
    return unit.save()
end

---Change priority.
---@param id string
---@param newprio string
---@return boolean
local function _set_prio(envname, id, newprio)
    unit.init(unit_dir .. utils.genname(envname, id))
    unit.set("prio", newprio)
    return unit.save()
end

function taskunit.init(unitdir, taskdir)
    unit_dir = unitdir
    task_dir = taskdir
    return true
end

---Check task units.
---@param key string
---@param value string
---@return boolean
function taskunit.check(key, value)
    local params = {
        uniqid = function(val)
            return not (val == nil or val == "")
        end,
        id = _check_id,
        desc = _check_desc,
        prio = _check_prio,
        type = _check_type,
        date = function(val)
            return not (val == nil or val == "")
        end,
        env = function(val)
            return not (val == nil or val == "")
        end,
    }

    -- Check a key-value pair.
    for name, func in pairs(params) do
        if name == key then
            return func(value)
        end
    end

    -- Unknown key, return false.
    return false
end

---Add a new unit for a task.
---@param id string
---@param desc string
---@param tasktype string
---@param prio string
---@return boolean
function taskunit.add(envname, id, desc, tasktype, prio)
    unit.init(unit_dir .. utils.genname(envname, id))
    unit.set("id", id)
    unit.set("env", envname)
    unit.set("prio", prio)
    unit.set("type", tasktype)
    unit.set("desc", desc)
    unit.set("date", tostring(os.date("%Y%m%d")))
    unit.set("uniqid", tostring(os.date("%s")))

    -- Check input values for validity.
    for _, ukey in pairs(unit.keys) do
        local value = unit.get(ukey.val)
        if not taskunit.check(ukey.val, value) then
            print("ERROR: taskunit.add: failed", ukey.val, value)
            return false
        end
    end

    -- Save values into the database.
    return unit.save()
end

---Set unit key value.
--Update related units as well.
---@param envname string
---@param id string
---@param key string
---@param value string
---@return boolean
function taskunit.set(envname, id, key, value)
    if key == "desc" then
        return _set_desc(envname, id, value)
    elseif key == "id" then
        return _set_id(envname, id, value)
    elseif key == "prio" then
        return _set_prio(envname, id, value)
    elseif key == "type" then
        return _set_type(envname, id, value)
    end
    return false
end

--- Delete task unit.
---@param envname string
---@param id string
---@return boolean
function taskunit.del(envname, id)
    local unitfile = unit_dir .. utils.genname(envname, id)
    return utils.rm(unitfile)
end

---Get unit from task metadata.
---@param id string
---@param key string
---@return string
function taskunit.get(envname, id, key)
    unit.init(unit_dir .. utils.genname(envname, id))
    return unit.get(key)
end

---Get task units.
---@param envname string
---@param id string
---@return table
function taskunit.units(envname, id)
    unit.init(unit_dir .. utils.genname(envname, id))
    return unit.units()
end

---Show task unit metadata.
---@param id string
---@param key string
---@param option string
---@return boolean
function taskunit.cat(envname, id, option, key)
    unit.init(unit_dir .. utils.genname(envname, id))

    -- output only key value
    if key then
        for _, ukey in pairs(unit.keys) do
            if ukey.val == key then
                local uval = unit.get(key)
                uval = uval and uval ~= "" and uval .. "\n" or ""
                io.write(("%s"):format(uval))
                return true
            end
        end
        return false
    end

    -- output all key values
    for _, ukey in pairs(unit.keys) do
        local value = unit.get(ukey.val) or ""
        if ukey.lvl == 1 then
            print(("%-8s: %s"):format(ukey.val, value))
        end
    end
    return true
end

return taskunit
