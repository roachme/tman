---Module that handles plugins.
-- @module plugin

local core = require("core.core")
local utils = require("aux.utils")
local config = require("aux.config")

local plugin = {
    struct = require("plugin.struct"),
    git = require("plugin.git"),
    make = require("plugin.make"),
}

function plugin.init(envname, id)
    -- git: make sure all repos are downloded


    local struct_dirs = config.user.get(envname).struct.dirs
    local struct_files = config.user.get(envname).struct.files

    local git_dirbase = config.user.get(envname).git.dirbase or "."
    local git_repos = config.user.get(envname).git.repos
    local git_repobase = core.struct.code.path
    local git_linkbase = core.struct.tasks.path .. envname .. "/" .. id .. "/"


    -- make sure plugin stuff don't conflict with each other.
    -- roachme: put a meaningful messeage here
    -- PART1: struct vs git
    for _, dir in pairs(struct_dirs) do
        if git_dirbase ~= "." and dir == git_dirbase then
            core.die(1, "plugins struct and git conflict", "plugin")
            return false
        end
    end
    for _, file in pairs(struct_files) do
        if git_dirbase ~= "." and file == git_dirbase then
            core.die(1, "plugins struct and git conflict", "plugin")
            return false
        end
    end


    -- prepare git
    if git_dirbase ~= "." then
        local dirbase = core.struct.tasks.path .. envname .. "/" .. id .. "/" .. git_dirbase
        if not utils.mkdir(dirbase) then
            return false
        end
    end

    plugin.struct.init()
    plugin.git.init(git_repos, git_repobase, git_linkbase, git_dirbase)
    plugin.make.init()
    return true
end

---Check that plugins are ok and don't conflict with each other.
---@return boolean
function plugin:check()
    return true
end

return plugin
