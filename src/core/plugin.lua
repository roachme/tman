---Provide system means to plugins.
--@module plugin

local core = require("core.core")
local config = require("aux.config")

local plugin = {}

plugin.prefix = core.struct.prefix.path
plugin.dbdir = core.struct.dbdir.path

---Get tasks units.
---@param envname string
---@param id string
---@return table
function plugin.getunits(envname, id)
    --[[
    local unit_dir = core.struct.units.path
    local task_dir = core.struct.tasks.path

    -- system dependant (fatal): load core modules
    if not taskid.init(core.struct.ids.path, core.struct.curr.path) then
        core.die(1, "could not init module taskid", "fatal")
        return {}
    elseif not taskunit.init(unit_dir, task_dir) then
        core.die(1, "could not init module taskunit", "fatal")
        return {}
    end

    return taskunit.units(envname, id)
    ]]
    return {}
end

---Get plugin config.
---@param envname string
---@return table
function plugin.get_uconfig(envname)
    return config.uget(envname)
end

return plugin
