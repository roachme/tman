--- Operate on task unit inside meta data.
-- Metadata like branch name, date, description and so on.
-- @module TaskUnit

local config = require("secondary.config")
local utils = require("aux.utils")
local unit = require("aux.unitdb")


-- roachme: gotta make sure that branch is correct.
-- so the rest of the code shouldn't check branch for nil.


local taskunit = {}

-- Private functions: end --

---Get user input.
---@param prompt string
---@return string
local function get_input(prompt)
    io.write(prompt, ": ")
    return io.read("*line")
end

---Generate unit filename.
---@param base string
---@param envname string
---@param id string
---@return string
local function genname(base, envname, id)
    return base .. "/" .. envname .. ":" .. id
end

---String separator.
---@param inputstr string
---@param sep string | nil
---@return table
local function pattsplit(inputstr, sep)
    local res = {}

    if sep == nil then
        sep = "%s"
    end
    for str in string.gmatch(inputstr, "([^" .. sep .. "]+)") do
        table.insert(res, str)
    end
    return res
end

---Form branch according to pattern.
---@return string | nil
local function format_branch()
    local separators = "/_-"
    local sepcomponents = pattsplit(config.user.branchpatt, separators)
    local branch = config.user.branchpatt

    for _, item in pairs(sepcomponents) do
        local uitem = unit.get(string.lower(item))

        if not uitem then
            local errmsg = "error: branch formatiton: '%s': unit not found\n"
            io.stderr:write(errmsg:format(item))
            return nil
        end

        -- roachme: HOTFIX: desc: replace whitespace with undrescore
        if item == "DESC" then
            uitem = string.gsub(uitem, " ", "_")
        end
        branch = string.gsub(branch, item, uitem)
    end
    return branch
end

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

---Check link.
---@param link string
---@return boolean
local function _check_link(link)
    local linkregex = "[a-zA-Z0-9_%s-:/.]*"
    link = link or "" -- roachme: find a better way.

    if string.match(link, linkregex) == link then
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
    unit.init(genname(config.core.units, envname, id))
    unit.set("desc", newdesc)
    unit.set("branch", format_branch())
    return unit.save()
end

---Change task ID.
---@param id string
---@param newid string
---@return boolean
local function _set_id(envname, id, newid)
    -- rocahme: outta use struct.lua
    local old_taskdir = config.aux.tasks .. envname .. ":" .. id
    local new_taskdir = config.aux.tasks .. envname .. ":" .. newid

    unit.init(genname(config.core.units, envname, id))
    unit.set("id", newid)
    unit.set("branch", format_branch())
    unit.save()

    -- gotta update curr & prev task IDs.
    return utils.rename(old_taskdir, new_taskdir)
end

---Change task type.
---@return boolean
local function _set_type(envname, id, newtype)
    unit.init(genname(config.core.units, envname, id))
    unit.set("type", newtype)
    unit.set("branch", format_branch())
    return unit.save()
end

---Change priority.
---@param id string
---@param newprio string
---@return boolean
local function _set_prio(envname, id, newprio)
    unit.init(genname(config.core.units, envname, id))
    unit.set("prio", newprio)
    return unit.save()
end

---Set link to work task manager.
---@param envname string
---@param id string
---@param newlink string
---@return boolean
local function _set_link(envname, id, newlink)
    unit.init(genname(config.core.units, envname, id))
    unit.set("link", newlink)
    return unit.save()
end

---Set list of active repos.
---@param id string
---@param taskrepos string
---@return boolean
local function _set_repo(envname, id, taskrepos)
    unit.init(genname(config.core.units, envname, id))
    unit.set("repos", taskrepos)
    return unit.save()
end

-- Private functions: end --

-- Public functions: start --

---Check task units.
---@param key string
---@param value string
---@return boolean
function taskunit.check(key, value)
    local params = {
        id = _check_id,
        desc = _check_desc,
        prio = _check_prio,
        type = _check_type,
        link = _check_link,
        branch = function(val)
            return not (val == nil or val == "")
        end,
        status = function(val)
            return not (val == nil or val == "")
        end,
        date = function(val)
            return not (val == nil or val == "")
        end,
        envname = function(val)
            return not (val == nil or val == "")
        end,

        -- roachme: under development.
        time = function()
            return true
        end,
        repos = function()
            return true
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
---@param tasktype string
---@param prio string
---@return boolean
function taskunit.add(envname, id, tasktype, prio)
    local branch
    local desc = get_input("Desc")
    prio = prio or unit.prios.mid

    -- Set values.
    unit.init(genname(config.core.units, envname, id))
    unit.set("id", id)
    unit.set("prio", prio)
    unit.set("type", tasktype)
    unit.set("desc", desc)
    unit.set("time", "")
    unit.set("link", "")
    unit.set("repos", "")
    unit.set("date", tostring(os.date("%Y%m%d")))
    unit.set("status", "progress")
    unit.set("envname", envname)

    branch = format_branch()
    if not branch then
        return false
    end

    unit.set("branch", branch)

    -- Check input values for validity.
    for _, ukey in pairs(unit.keys) do
        local value = unit.get(ukey)
        if not taskunit.check(ukey, value) then
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
    elseif key == "link" then
        return _set_link(envname, id, value)
    elseif key == "prio" then
        return _set_prio(envname, id, value)
    elseif key == "repos" then
        return _set_repo(envname, id, value)
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
    local unitfile = genname(config.core.units, envname, id)
    return utils.rm(unitfile)
end

---Get unit from task metadata.
---@param id string
---@param key string
---@return string
function taskunit.get(envname, id, key)
    unit.init(genname(config.core.units, envname, id))
    return unit.get(key)
end

---Show task unit metadata.
---@param id string
---@param key string
---@return boolean
function taskunit.cat(envname, id, key)
    unit.init(genname(config.core.units, envname, id))

    -- output only key value
    if key then
        for _, ukey in pairs(unit.keys) do
            if ukey == key then
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
        local value = unit.get(ukey) or ""
        print(("%-8s: %s"):format(ukey, value))
    end
    return true
end

-- Public functions: end --

return taskunit
