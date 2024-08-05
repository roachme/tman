local taskenv = require("core.taskenv")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local getopt = require("posix.unistd").getopt
local utils = require("aux.utils")

local envname

---Set task description.
---@param id string
---@param newdesc string
---@return boolean
local function _set_desc(id, newdesc)
    local olddesc = taskunit.get(envname, id, "desc")

    if olddesc == newdesc then
        core.die(1, "the same task description", newdesc)
    end

    -- roachme: the only reasons why it might fail
    -- 1. Dir doesn't exist.
    -- 2. Has no permition.
    -- 3. Hardware isssue.
    -- core.lua gotta check that out, so we ain't gotta check, just do it.
    if not taskunit.set(envname, id, "desc", newdesc) then
        core.die(1, "could not set new task description", "desc")
    end
    return true
end

---Set task id.
---@param id string
---@param newid string
local function _set_id(id, newid)
    if id == newid then
        -- do nothing.
        return 0
    elseif taskid.exist(envname, newid) then
        return core.die(1, "task id exists", newid)
    elseif not taskunit.set(envname, id, "id", newid) then
        return core.die(1, "could not set new task id", newid)
    end

    taskid.rename(envname, id, newid)
    -- cache current task id
    taskid.updcurr(envname)

    local olddir = core.struct.units.path .. envname .. "/" .. id
    local newdir = core.struct.units.path .. envname .. "/" .. newid
    if not utils.rename(olddir, newdir) then
        return core.die(1, "could not rename task directory", "set")
    end
    return true
end

---Set task priority.
---@param id string
---@param newprio string
---@return boolean
local function _set_prio(id, newprio)
    local prio = taskunit.get(envname, id, "prio")

    if newprio == prio then
        local errfmt = "'%s' and '%s' are the same task priority"
        core.die(1, errfmt, "set", prio, newprio)
    elseif not taskunit.set(envname, id, "prio", newprio) then
        core.die(1, "could not set new priority", newprio)
    end
    return true
end

---Set task type.
---@param id string
---@param newtype string
---@return boolean
local function _set_type(id, newtype)
    local currtype = taskunit.get(envname, id, "type")
    taskunit.set(envname, id, "type", newtype)

    if currtype == newtype then
        local errfmt = "'%s' and '%s' are the same task type"
        core.die(1, errfmt, "set", currtype, newtype)
        return true
    end
    return true
end

---Set task unit value.
local function builtin_set()
    local id
    local last_index = 1
    local optstr = "di:p:t:"
    local options = {
        newdesc = { key = "desc", arg = nil, func = _set_desc },
        newprio = { key = "prio", arg = nil, func = _set_prio },
        newtype = { key = "type", arg = nil, func = _set_type },
        newid = { key = "id", arg = nil, func = _set_id },
    }

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            core.die(1, "unrecognized option", arg[optind - 1])
        end

        last_index = optind
        if optopt == "d" then
            if not taskunit.check("desc", optarg) then
                core.die(1, "invalid description", optarg)
            end
            options.newdesc.arg = optarg
        elseif optopt == "i" then
            if not taskunit.check("id", optarg) then
                core.die(1, "invalid id", optarg)
            end
            options.newid.arg = optarg
        elseif optopt == "p" then
            if not taskunit.check("prio", optarg) then
                core.die(1, "invalid priority", optarg)
            end
            options.newprio.arg = optarg
        elseif optopt == "t" then
            if not taskunit.check("type", optarg) then
                core.die(1, "invalid task type", optarg)
            end
            options.newtype.arg = optarg
        end
    end

    id = arg[last_index]
    envname = arg[last_index + 1]

    for _, item in pairs(options) do
        if item.arg then
            core.set(envname, id, item.key, item.arg)
        end
    end
    return 0
end

return builtin_set
