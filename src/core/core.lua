--- Tman core module to to init, check and repair itself.
-- @module core

--[[

Each command in this module can be executed separately.
And the point is a user (builtin commands) cannot break anything.

]]

local comm = require("core.comm")
local utils = require("aux.utils")
local switch = require("core.switch")
local taskenv = require("core.taskenv")
local taskunit = require("core.taskunit")
local config = require("aux.config")

local core = {}

local prefix = config.prefix

local struct = {
    prefix = { isdir = false, iscore = true, path = prefix },
    dbdir = { isdir = false, iscore = true, path = prefix .. "/.tman/" },
    units = { isdir = true, iscore = true, path = prefix .. "/.tman/units/" },
    refs = { isdir = true, iscore = true, path = prefix .. "/.tman/refs/" },
    tasks = { isdir = true, iscore = false, path = prefix .. "/tasks/" },
    envs = { isdir = false, iscore = true, path = prefix .. "/.tman/refs/envs" },
    curr = { isdir = false, iscore = true, path = prefix .. "/.tman/curr" },
    prev = { isdir = false, iscore = true, path = prefix .. "/.tman/prev" },
    ids = { isdir = false, iscore = true, path = prefix .. "/.tman/refs/ids" },
}

---comment
---@param exit_code number
---@param errfmt string
---@param ... unknown
---@return number
function core.die(exit_code, errfmt, ...)
    local errmsg = ("%s: %s: " .. errfmt .. "\n"):format(progname, ...)
    io.stderr:write(errmsg)
    os.exit(exit_code)
end

---Check tman core and base directories ain't corrupted and exist.
---@return boolean
function core.check()
    for _, file in pairs(struct) do
        local path = file.path
        if not utils.access(path) then
            return false
        end
    end
    return true
end

---Create tman core and base directories.
---@return boolean
function core.create()
    for _, file in pairs(struct) do
        local path = file.path
        if file.isdir then
            if not utils.mkdir(path) then
                return false
            end
        end
    end

    for _, file in pairs(struct) do
        local path = file.path
        if not file.isdir then
            if not utils.touch(path) then
                return false
            end
        end
    end
    return true
end

--- Backup data.
-- @param fname archive filename (default extention is .tar)
-- @param repo_included whether or not include repos in archive
-- @return on success - true
-- @return on failure - false
function core.backup(fname, repo_included)
    -- roachme: run git gc before including repos in archive so it takes less place.
    -- roachme: Replace codebase with value from config
    local cmd

    if repo_included then
        cmd = ("tar -C %s -czf %s.tar ."):format(config.base, fname)
    else
        cmd = ("tar -C %s --exclude=codebase -czf %s.tar ."):format(
            config.base,
            fname
        )
    end
    return utils.exec(cmd)
end

--- Restore archive.
-- @param fname archive fname
-- @return on success - true
-- @return on failure - false
function core.restore(fname)
    local cmd = ("tar -xf %s -C %s"):format(fname, config.base)

    if not utils.access(fname) then
        io.stderr:write(("'%s': no such tar file\n"):format(fname))
        return false
    end

    print("delete current structure")
    utils.rm(config.taskbase)
    utils.rm(config.codebase)
    utils.rm(config.tmanbase)

    print("copy backup structure")
    if not utils.exec(cmd) then
        io.stderr:write("failed to execute archive command\n")
        return false
    end
    return true
end

--[[
Found the logic in git project. File name setup.c:
    1. Run this module before any command. Pro'ly all but might use a flag to
       specify it.
    2. Make sure tman directory is safe to perform any command. It frees the
       rest of code logic from checks and crap like that.
]]

--[[

    1. Check that every task id has corresponding. Yeah, all. It shouldn't take
       that much time is it seems. Tho the rest of the code'll run no error.
       If it slows down performance, rewrite it in C. Sounds good?
        a) unit file
        b) task dir
]]

---Init core modules.
function core.init()
    if not taskenv.init(struct.envs.path) then
        return core.die(1, "could not init module taskenv", "fatal")
    elseif not taskunit.init(struct.units.path) then
        return core.die(1, "could not init module taskunit", "fatal")
    elseif not switch.init(struct.dbdir.path) then
        return core.die(1, "could not init module switch", "fatal")
    end
end

--[[
setup levels
0 - no setup. Command `init' need, to create tman structure.
1 - gentle setup. Jost make sure tman structure is created and ok.
2 - gentle setup and database's ok.
3 - all above and config files ok.
]]
---Basic system check (level: 1).
local function setup_basic()
    if not core.check() then
        return core.die(1, "tman not inited", "setup")
    end

    core.init()
    return 0
end

---Strict system check (level: 2).
local function setup_strict()
    setup_basic()

    -- TODO: make sure database ain't corrupted.
end

---Full system and user lands check (level: 3).
local function setup_full()
    setup_strict()

    -- TODO: make sure (user) config stuff are valid
    -- user custom stuff make sense like brachpatt, etc.
end

---Setup database stuff before execute a command.
---@param level number
function core.setup(level)
    if level == 0 then
        -- do nothing
        return 0
    elseif level == 1 then
        return setup_basic()
    elseif level == 2 then
        return setup_strict()
    elseif level == 3 then
        return setup_full()
    end
    return core.die(1, "no such setup level '%d'", "setup", level)
end

--[[

core: setup & modules
builtins
plugins

file structure:
curr refs/ units/ pgnunits/

]]

---Add a new environment.
---@param envname string
---@param desc string?
---@return boolean
function core.env_add(envname, desc)
    desc = desc or "autogenerated environment description"

    if not envname then
        core.die(1, "environment name required", "envadd")
    elseif not taskenv.chk(envname) then
        core.die(1, "illegal environment name", "env")
    elseif taskenv.ext(envname) then
        core.die(1, "environment name already exists", envname)
    elseif not taskenv.add(envname, desc) then
        core.die(1, "could not add new environment", envname)
    elseif not switch.env_addcurr({ env = envname }) then
        core.die(1, "could not set current environment", "add")
    end

    -- create needed structure
    if not utils.mkdir(struct.units.path .. "/" .. envname) then
        local errfmt = "could not create environment directory in units"
        core.die(1, errfmt, "env")
    elseif not utils.mkdir(struct.tasks.path .. "/" .. envname) then
        local errfmt = "could not create task environment directory"
        core.die(1, errfmt, "env")
    end
    return true
end

---Get current environment name.
---@return string | nil
function core.env_curr()
    return switch.env_getcurr().env
end

---Delete an environment.
---@param envname string
---@return boolean
function core.env_del(envname)
    local curr
    envname = envname or switch.env_getcurr().env

    if not envname then
        core.die(1, "no current environment", "envdel")
    elseif not taskenv.ext(envname) then
        core.die(1, "no such environment name", envname)
    elseif not taskenv.del(envname) then
        core.die(1, "could not delete environment name", envname)
    elseif not utils.rm(struct.units.path .. "/" .. envname) then
        core.die(1, "could not delete environment name in units", envname)
    elseif not utils.rm(struct.tasks.path .. "/" .. envname) then
        core.die(1, "could not delete environment tasks", envname)
    end

    -- update current environment and task id if needed.
    curr = switch.env_getcurr().env
    if curr then
        switch.id_delcurr()
        if switch.id_getcurr() then
            switch.id_delcurr()
        end
    end
    return true
end

---Get list of all environments.
---@return table
function core.env_list()
    local curr = switch.env_getcurr().env
    local prev = switch.env_getprev().env

    -- roachme: add curr & prev environment statuses from module switch.lua
    -- code goes here...

    return taskenv.list()
end

---Switch to environment.
---@param envname string
function core.env_switch(envname)
    if not envname then
        core.die(1, "environment name required", "envswitch")
        return 1
    elseif not taskenv.ext(envname) then
        core.die(1, "no such environment name", envname)
        return 1
    elseif not switch.env_addcurr({ envname }) then
        core.die(1, "could not set new current environment", envname)
        return 1
    elseif not switch.env_addcurr({ env = envname }) then
        core.die(1, "could not set update curr file", envname)
        return 1
    end
    return 0
end

---Switch to previous environment.
function core.env_prev()
    local prevenv = switch.env_getcurr()

    if not prevenv.prev then
        core.die(1, "no previous environment", "prev")
        return 1
    end
    switch.env_swapspec()
    return 0
end

---Add a new taks.
---@param envname string?
---@param id string
---@return boolean
function core.id_add(envname, id, units)
    core.init()
    envname = envname or switch.env_getcurr().env

    if not envname then
        core.die(1, "no current environment", "envname")
        return false
    elseif not id then
        core.die(1, "task id required", "id")
        return false
    end

    -- make sure needed environment is ready.
    if not taskenv.ext(envname) then
        core.env_add(envname)
    end
    if switch.env_getcurr().env ~= envname then
        if not switch.env_addcurr({ envname }) then
            core.die(1, "could not set current environment", "add")
        end
    end

    local unitdir = struct.units.path .. "/" .. envname .. "/" .. id
    if not utils.mkdir(unitdir) then
        taskunit.del(envname, id)
        core.die(1, "fatal never: could not create unit dir", "unitdir")
        return false
    end

    if taskunit.ext(envname, id) then
        local errfmt = "task id %s already exists in environment %s"
        core.die(1, errfmt:format(id, envname), "fatal")
        return false
    elseif not taskunit.add(envname, id) then
        local errfmt = "could not add task id units %s to environment %s"
        core.die(1, errfmt:format(id, envname), "fatal")
        return false
    elseif not switch.id_addcurr(id) then
        local errfmt = "could not set current task id units %s to environment %s"
        core.die(1, errfmt:format(id, envname), "fatal")
        return false
    end

    local taskdir = struct.tasks.path .. "/" .. envname .. "/" .. id
    if not utils.mkdir(taskdir) then
        taskunit.del(envname, id)
        core.die(1, "fatal never: could not create task dir", "taskdir")
        return false
    end

    return true
end

---Delete task.
---@param envname string
---@param id string
---@return boolean
function core.id_del(envname, id)
    core.init()
    local curr = switch.id_getcurr()
    local prev = switch.id_getprev()

    -- check input values
    envname = envname or switch.env_getcurr().env
    if not envname then
        core.die(1, "no current environment", "envname")
        return false
    elseif not taskenv.ext(envname) then
        local errfmt = "no such environment name"
        core.die(1, errfmt, "envname")
        return false
    end

    id = id or switch.id_getcurr()
    if not id then
        core.die(1, "no current task id", "id")
        return false
    elseif not taskunit.ext(envname, id) then
        local errfmt = "no such task id in environment %s"
        core.die(1, errfmt, id, envname)
        return false
    elseif not taskunit.del(envname, id) then
        local errfmt = "could not delete task id in environment %s"
        core.die(1, errfmt, id, envname)
        return false
    elseif id == curr then
        switch.id_delcurr()
    elseif id == prev then
        -- cuz there's no way to delete previous task id directly
        switch.id_swapspec()
        switch.id_delcurr()
    end

    -- actual logic
    local taskdir = struct.tasks.path .. "/" .. envname .. "/" .. id
    if not utils.rm(taskdir) then
        core.die(1, "fatal never: could not delete task dir", "taskdir")
        return false
    end
    return true
end

local function check_input(envname, id)
end

---Set task units.
---@param envname string
---@param id string
---@param key string
---@param val string
---@return boolean
function core.id_set(envname, id, key, val)
    local currid = switch.id_getcurr()
    local previd = switch.id_getprev()

    core.init()

    -- check input values
    envname = envname or switch.env_getcurr().env
    if not envname then
        core.die(1, "no current environment", "envname")
        return false
    elseif not taskenv.ext(envname) then
        local errfmt = "no such environment name"
        core.die(1, errfmt, "envname")
        return false
    end

    id = id or switch.id_getcurr()
    if not id then
        core.die(1, "task id required", "id")
        return false
    elseif not taskunit.ext(envname, id) then
        local errfmt = "no such task id in environment %s"
        core.die(1, errfmt, id, envname)
        return false
    end

    if not taskunit.set(envname, id, key, val) then
        core.die(1, "could not set unit value", id)
    end

    if key == "id" then
        local old_taskdir = struct.tasks.path .. "/" .. envname .. "/" .. id
        local new_taskdir = struct.tasks.path .. "/" .. envname .. "/" .. val
        if not utils.rename(old_taskdir, new_taskdir) then
            core.die(1, "could not rename task directory", val)
            return false
        end
    end

    return true
end

---Switch to task.
---@param envname string
---@param id string
---@return boolean
function core.id_switch(envname, id)
    core.init()
    envname = envname or switch.env_getcurr().env

    if not taskenv.ext(envname) then
        core.die(1, "no such environment", envname)
    elseif not switch.env_addcurr({ envname }) then
        core.die(1, "could not switch to environment %s", envname, "switch")
    elseif not taskunit.ext(envname, id) then
        core.die(1, "no such task id in environment %s", id, envname)
    elseif not switch.id_addcurr(id) then
        core.die(1, "could not set current task %s", id, envname)
    end
    return true
end

---Switch to previous task.
---@param envname string?
function core.id_prev(envname)
    core.init()
    local prev

    envname = envname or switch.env_getcurr().env
    if not envname then
        core.die(1, "no current environment", "env")
        return false
    elseif not taskenv.ext(envname) then
        core.die(1, "no such environment name", envname)
        return false
    elseif switch.env_getcurr().env ~= envname then
        if not switch.env_addcurr({ env = envname }) then
            core.die(1, "could not set current environment", "add")
        end
    end

    prev = switch.id_getprev()
    if not prev then
        core.die(1, "no previos task in environment %s", "prev", envname)
        return false
    elseif not switch.id_swapspec() then
        local errfmt = "could not switch to previous task in environment %s"
        core.die(1, errfmt, "prev", envname)
        return false
    end
    return true
end

---Get list of tasks.
---@param envname string
---@return table
function core.id_list(envname)
    core.init()
    local tasks = {}
    local currid = switch.id_getcurr()
    local previd = switch.id_getprev()
    envname = envname or switch.env_getcurr().env

    if not envname then
        core.die(1, "no current environment", "list")
        return {}
    elseif not taskenv.ext(envname) then
        core.die(1, "no such environment name %s", "list", envname)
        return {}
    end

    local taskids = taskunit.list(envname)
    for _, id in pairs(taskids) do
        local units = taskunit.get(envname, id)
        local status
        -- roachme: gotta simplify it
        if id == currid then
            status = 0
        elseif id == previd then
            status = 1
        else
            status = 2
        end
        table.insert(tasks, {id = id, desc = units.desc, status = status})
    end
    return tasks
end

---Move task into another environment.
---@param envname string
---@param id string
function core.id_move(envname, id)
    core.init()
    local currenv = switch.env_getcurr().env

    if not envname then
        core.die(1, "environment name required", "move")
    elseif not taskenv.ext(envname) then
        core.die(1, "no such environment name", envname)
    end
end

---Get task units.
---@param envname string
---@param id string
---@return table
function core.id_cat(envname, id)
    core.init()

    envname = envname or switch.env_getcurr().env
    if not envname then
        core.die(1, "no current environment", "cat")
        return {}
    elseif not taskenv.ext(envname) then
        core.die(1, "no such environment %s", "cat", envname)
        return {}
    end

    id = id or switch.id_getcurr()
    if not id then
        core.die(1, "no current task id in environment %s", "cat", envname)
        return {}
    elseif not taskunit.ext(envname, id) then
        core.die(1, "no such task id in environment %s", "cat", envname)
        return {}
    end

    return taskunit.get(envname, id)
end

function core.id_curr(envname)
    envname = envname or core.env_curr()

    if not envname then
        core.die(1, "no current environment", "env")
        return nil
    end
    return switch.id_getcurr()
end

function core.getunits(envname, id)
    envname = envname or core.env_curr()

    if not envname then
        core.die(1, "no current envname", "env")
        return {}
    end
    id = id or core.id_curr(envname)
    if not id then
        core.die(1, "no current task id", "")
        return {}
    end

    return taskunit.get(envname, id)
end

core.prefix = prefix
core.dbdir = struct.dbdir.path

return core
