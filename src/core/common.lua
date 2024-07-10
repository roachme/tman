--- Common module for the rest of commands.
-- @module common

local env = require("core.env")
local git = require("secondary.git")
local help = require("secondary.help")
local config = require("secondary.config")
local struct = require("secondary.struct")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")

local common = {}

function common.die(exit_code, errfmt, ...)
    local errmsg = ("%s: %s: " .. errfmt):format(help.progname, ...)
    io.stderr:write(errmsg)
    os.exit(exit_code)
end

function common.die_atomic(id, errfmt, ...)
    local envname = env.getcurr()

    if not envname then
        return common.die(1, "no current env\n", "env")
    end

    taskid.init(config.core.refs.ids)
    local branch = taskunit.get(envname, id, "branch")

    if not branch then
        return common.die(1, "no branch\n", id)
    end

    local path = config.aux.code
    for _, repo in pairs(config.user.repos) do
        git.branch_delete(repo.name, branch, path)
    end

    taskid.del(envname, id)
    taskunit.del(envname, id)
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
