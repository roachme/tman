---Provide system means to plugins.
--@module plugin

local core = require("core.core")
local config = require("aux.config")

local plugin = {}

plugin.prefix = core.prefix
plugin.dbdir = core.dbdir

---Get tasks units.
---@param envname string
---@param id string
---@return table
function plugin.getunits(envname, id)
    return core.id_cat(envname, id)
end

---Get plugin config.
---@param envname string
---@return table
function plugin.get_uconfig(envname)
    return config.uget(envname)
end

return plugin
