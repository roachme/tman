---Provide system means to plugins.
--@module plugin

local core = require("core.core")
local config = require("aux.config")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")

local plugin = {}

function plugin.init(envname, id)
    local unit_dir = core.struct.units.path
    local task_dir = core.struct.tasks.path

    -- system dependant (fatal): load core modules
    if not taskid.init(core.struct.ids.path) then
        return core.die(1, "could not init module taskid", "fatal")
    elseif not taskunit.init(unit_dir, task_dir) then
        return core.die(1, "could not init module taskunit", "fatal")
    end

    plugin.prefix = core.struct.prefix.path
    plugin.uconfig = config.uget(envname)
    plugin.units = taskunit.units(envname, id)
    return true
end

---Get task units.
---@param envname string
---@param uniqid string
---@return table
function plugin.get_units(envname, uniqid)
    return {}
end

---Get plugin config.
---@param envname string
---@return table
function plugin.get_uconfig(envname)
    return config.uget(envname)
end

return plugin
