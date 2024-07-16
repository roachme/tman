---Module that handles plugins.
-- @module plugin

local plugin = {
    struct = require("plugin.struct"),
    git = require("plugin.git"),
    make = require("plugin.make"),
}

return plugin
