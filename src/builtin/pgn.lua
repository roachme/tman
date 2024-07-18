local core = require("core.core")

local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local taskenv = require("core.env")
local config = require("aux.config")
local git = require("plugin.git")
--local make = require("plugin.make")
local struct = require("plugin.struct")

local envname
local id

local function _plugin_struct(cmd)
    local taskbase = core.struct.tasks.path .. "/" .. envname
    local dirs = config.user.get(envname).struct.dirs
    local files = config.user.get(envname).struct.files

    if not struct.init(taskbase, dirs, files) then
        return core.die(1, "could not init struct plugin", "struct")
    end

    if cmd == "create" then
        struct.create(id)
    elseif cmd == "delete" then
        struct.delete(id)
    end
end

local function _plugin_git(cmd, units)
    --[[
    local git_dirbase = config.user.get(envname).git.dirbase or "."
    local git_repos = config.user.get(envname).git.repos
    local git_repobase = core.struct.code.path
    local git_linkbase = core.struct.tasks.path .. envname
    local git_commitpatt = config.user.get(envname).git.commitpatt
    local git_branchpatt = config.user.get(envname).git.branchpatt
    git.init(git_repos, git_repobase, git_linkbase, git_dirbase, git_commitpatt, git_branchpatt)
    ]]

    if cmd == "sync" then
        git.sync(envname, id)

    elseif cmd == "create" then
        git.create(id, units)

    elseif not cmd then
        print("error: command required")
    else
        print("no such command", cmd)
    end
end

--local function _plugin_make(cmd) end

local function builtin_plugin()
    local plugin = arg[1]
    local cmd = arg[2] or "create"

    if not taskenv.init(core.struct.envs.path) then
        return core.die(1, "could not init module taskenv", "fatal")
    elseif not taskid.init(core.struct.ids.path) then
        return core.die(1, "could not init module taskid", "fatal")
    elseif not taskunit.init(core.struct.units.path) then
        return core.die(1, "could not init module taskunit", "fatal")
    end

    envname = taskenv.getcurr()
    if not envname then
        return core.die(1, "no current environment", "env")
    elseif not taskenv.exist(envname) then
        return core.die(1, "no such environment", "env")
    end

    id = taskid.getcurr(envname)
    if not id then
        return core.die(1, "no current task id", "id")
    end

    if plugin == "struct" then
        _plugin_struct(cmd)
    elseif plugin == "git" then
        _plugin_git(cmd, taskunit.units(envname, id))
    elseif not plugin then
        core.die(1, "plugin name required", "")
    else
        core.die(1, "no such plugin", plugin)
    end
end

return builtin_plugin
