--- Common module for the rest of commands.
-- @module common

local git = require("core.git")
local help = require("core.help")
local struct = require("core.struct")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")

local common = {}

function common.die(exit_code, errfmt, ...)
    local errmsg = ("%s: %s: " .. errfmt):format(help.progname, ...)
    io.stderr:write(errmsg)
    os.exit(exit_code)
end

function common.die_atomic(id, errfmt, ...)
    taskid.del(id)
    taskunit.del(id)
    git.branch_delete(id)
    struct.delete(id)
    common.die(1, errfmt, ...)
end

---Generate unit filename.
---@param envname string
---@param id string
---@return string
function common.genname(envname, id)
    return envname .. ":" .. id
end

return common
