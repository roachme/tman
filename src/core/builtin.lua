-- Module to hold all builtin commands.
-- @module builtin

local builtin = {
    { name = "add", command = require("builtin.add"), setup_level = 1 },
    { name = "arch", command = require("builtin.arch"), setup_level = 1 },
    { name = "cat", command = require("builtin.cat"), setup_level = 1 },
    { name = "cfg", command = require("builtin.cfg"), setup_level = 1 },
    { name = "del", command = require("builtin.del"), setup_level = 1 },
    { name = "env", command = require("builtin.env"), setup_level = 1 },
    { name = "help", command = require("builtin.help"), setup_level = 0 },
    { name = "init", command = require("builtin.init"), setup_level = 0 },
    { name = "list", command = require("builtin.list"), setup_level = 1 },
    { name = "pgn", command = require("builtin.pgn"), setup_level = 1 },
    { name = "prev", command = require("builtin.prev"), setup_level = 1 },
    { name = "set", command = require("builtin.set"), setup_level = 1 },
    { name = "sync", command = require("builtin.sync"), setup_level = 1 },
    { name = "use", command = require("builtin.use"), setup_level = 1 },
    { name = "ver", command = require("builtin.ver"), setup_level = 0 },
}

return builtin
