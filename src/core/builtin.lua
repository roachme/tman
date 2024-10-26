-- Module to hold all builtin commands.
-- @module builtin

local builtin = {
    { name = "add", command = require("src.builtin.add"), setup_level = 1 },
    { name = "arch", command = require("src.builtin.arch"), setup_level = 1 },
    { name = "cat", command = require("src.builtin.cat"), setup_level = 1 },
    { name = "cfg", command = require("src.builtin.cfg"), setup_level = 0 },
    { name = "del", command = require("src.builtin.del"), setup_level = 1 },
    { name = "env", command = require("src.builtin.env"), setup_level = 1 },
    { name = "help", command = require("src.builtin.help"), setup_level = 0 },
    { name = "init", command = require("src.builtin.init"), setup_level = 0 },
    { name = "list", command = require("src.builtin.list"), setup_level = 1 },
    { name = "move", command = require("src.builtin.move"), setup_level = 1 },
    { name = "pgn", command = require("src.builtin.pgn"), setup_level = 1 },
    { name = "prev", command = require("src.builtin.prev"), setup_level = 1 },
    { name = "set", command = require("src.builtin.set"), setup_level = 1 },
    { name = "sync", command = require("src.builtin.sync"), setup_level = 1 },
    { name = "use", command = require("src.builtin.use"), setup_level = 1 },
    { name = "ver", command = require("src.builtin.ver"), setup_level = 0 },
}

return builtin
