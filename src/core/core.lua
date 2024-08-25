--- Tman core module to to init, check and repair itself.
-- @module core

--[[

Each command in this module can be executed separately.
And the point is a user (builtin commands) cannot break anything.

]]

--local comm = require("core.comm")
local utils = require("aux.utils")
local errmod = require("core.errmod")
local switch = require("core.switch")
local taskenv = require("core.taskenv")
local taskdir = require("core.taskdir")
local taskunit = require("core.taskunit")
local config = require("aux.config")

local core = {}

local prefix = config.prefix

local struct = {
    prefix = { isdir = false, iscore = true, path = prefix },
    dbdir = { isdir = false, iscore = true, path = prefix .. "/.tman/" },
    units = { isdir = true, iscore = true, path = prefix .. "/.tman/units/" },
    tasks = { isdir = true, iscore = false, path = prefix .. "/tasks/" },
    curr = { isdir = false, iscore = true, path = prefix .. "/.tman/curr" },
    prev = { isdir = false, iscore = true, path = prefix .. "/.tman/prev" },
}

---Die to util.
---@param exit_code number
---@param errfmt string
---@param ... unknown
---@return number
function core.die(exit_code, errfmt, ...)
    local errmsg = ("%s: '%s': " .. errfmt .. "\n"):format(program, ...)
    io.stderr:write(errmsg)
    os.exit(exit_code)
end

---Check passed arguments.
---@param env table
---@param id table
local function chkargs(env, id)
    if env.chk and not env.val then
        core.die(1, errmod.EEREQ, "env")
    elseif env.chk and not taskenv.chk(env.val) then
        core.die(1, errmod.EEILL, env.val)
    elseif env.chk and not taskenv.ext(env.val) then
        core.die(1, errmod.EENON, env.val)
    elseif id.chk and not id.val then
        core.die(1, errmod.EIREQ, "id")
    elseif id.chk and not taskunit.chk("id", id.val) then
        core.die(1, errmod.EIILL, id.val)
    elseif id.chk and not taskunit.ext(env.val, id.val) then
        core.die(1, errmod.EINON, id.val)
    end
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

---Init core modules.
function core.init()
    if not taskenv.init(struct.units.path) then
        return core.die(1, "could not init module taskenv", "fatal")
    elseif not taskunit.init(struct.units.path) then
        return core.die(1, "could not init module taskunit", "fatal")
    elseif not switch.init(struct.dbdir.path) then
        return core.die(1, "could not init module switch", "fatal")
    elseif not taskdir.init(struct.tasks.path) then
        return core.die(1, "could not init module taskdir", "fatal")
    end
    return 0
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
    return core.init()
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

---Add a new environment.
---@param env string
---@return boolean
function core.env_add(env)
    if not env then
        core.die(1, errmod.EEREQ, "env")
    elseif not taskenv.chk(env) then
        core.die(1, errmod.EEILL, env)
    elseif taskenv.ext(env) then
        core.die(1, errmod.EEEXT, env)
    elseif not taskenv.add(env) then
        core.die(1, errmod.EEADD, env)
    elseif not switch.env_addcurr({ env = env }) then
        core.die(1, errmod.EESET, env)
    end

    -- create needed structure
    if not utils.mkdir(struct.units.path .. "/" .. env) then
        local errfmt = "could not create environment directory in units"
        core.die(1, errfmt, "env")
    elseif not utils.mkdir(struct.tasks.path .. "/" .. env) then
        local errfmt = "could not create task environment directory"
        core.die(1, errfmt, "env")
    end
    return true
end

---Delete an environment.
---@param env string
---@return boolean
function core.env_del(env)
    local curr = switch.env_getcurr().env
    local prev = switch.env_getprev().env
    env = env or switch.env_getcurr().env

    if not env then
        core.die(1, errmod.EECUR, "env")
    elseif not taskenv.ext(env) then
        core.die(1, errmod.EENON, env)
    elseif not taskenv.del(env) then
        core.die(1, errmod.EEDEL, "envdel")
    elseif not utils.rm(struct.units.path .. "/" .. env) then
        core.die(1, errmod.EEUDEL, env)
    elseif not utils.rm(struct.tasks.path .. "/" .. env) then
        core.die(1, errmod.EETDEL, env)
    end

    -- update special environments.
    if env == curr then
        switch.env_delcurr()
    elseif env == prev then
        -- cuz there's no way to delete previous task id directly
        switch.env_swapspec()
        switch.env_delcurr()
    end
    return true
end

---Get list of environments.
---@return table
function core.env_list()
    local res = {}
    local curr = switch.env_getcurr().env
    local prev = switch.env_getprev().env

    -- roachme:TODO simplify it
    for _, name in pairs(taskenv.list()) do
        local status
        if curr and curr == name then
            status = 0
        elseif prev and prev == name then
            status = 1
        else
            status = 2
        end
        table.insert(res, { name = name, status = status })
    end
    return res
end

---Switch to another environment.
---@param env string
function core.env_switch(env)
    local currenv = switch.env_getcurr().env

    if currenv == env then
        -- do nothing
        return true
    elseif not env then
        core.die(1, errmod.EEREQ, "env")
    elseif not taskenv.ext(env) then
        core.die(1, errmod.EENON, env)
    elseif not switch.env_addcurr({ env = env }) then
        core.die(1, errmod.EESET, env)
    end
    return true
end

---Switch to previous environment.
function core.env_prev()
    if not switch.env_getprev().env then
        core.die(1, errmod.EEPRV, "env")
    elseif not switch.env_swapspec() then
        core.die(1, errmod.EEPRS, "env")
    end
    return 0
end

---Get current environment and special task ids.
---@return table
function core.getcurr()
    return switch.env_getcurr()
end

---Add a new taks.
---@param env string?
---@param id string
---@return boolean
function core.id_add(env, id)
    env = env or switch.env_getcurr().env

    -- check input values.
    if not env then
        core.die(1, errmod.EECUR, "env")
    elseif not taskenv.ext(env) then
        core.die(1, errmod.EENON, env)
    elseif not taskenv.chk(env) then
        core.die(1, errmod.EEILL, env)
    elseif taskunit.ext(env, id) then
        core.die(1, errmod.EIEXT, id)
    elseif not taskunit.chk("id", id) then
        core.die(1, errmod.EIILL, id)
    end

    -- roachme: gotta output error description accepted from module
    if not taskunit.add(env, id) then
        core.die(1, errmod.EIEXT, id)
    elseif not switch.id_addcurr(id) then
        core.die(1, errmod.EISET, id)
    elseif not taskdir.add(env, id) then
        core.die(1, errmod.EETADD, id)
    end
    return true
end

---Delete task.
---@param env string
---@param id string
---@return boolean
function core.id_del(env, id)
    local currenv = switch.env_getcurr()
    local curr = currenv.curr
    local prev = currenv.prev
    env = env or currenv.env
    id = id or currenv.curr

    -- check input values
    chkargs({ val = env, chk = true }, { val = id, chk = true })

    if not taskunit.del(env, id) then
        core.die(1, errmod.EIDEL, id)
    elseif id == curr then
        switch.id_delcurr()
    elseif id == prev then
        -- cuz there's no way to delete previous task id directly
        switch.id_swapspec()
        switch.id_delcurr()
    elseif not taskdir.del(env, id) then
        core.die(1, errmod.EETDEL, id)
    end
    return true
end

---Set task units.
---@param env string
---@param id string
---@param key string
---@param val string
---@return boolean
function core.id_set(env, id, key, val)
    local envenv = switch.env_getcurr()
    env = env or envenv.env
    id = id or envenv.curr
    local currid = envenv.curr
    local previd = envenv.prev

    -- check input values
    if not env then
        core.die(1, errmod.EECUR, "env")
    elseif not taskenv.ext(env) then
        core.die(1, errmod.EENON, env)
    elseif not id then
        core.die(1, errmod.EIREQ, "id")
    elseif not taskunit.ext(env, id) then
        core.die(1, errmod.EINON, id)
    end

    if not taskunit.set(env, id, key, val) then
        core.die(1, errmod.EEUSET, id)
    end

    -- update some stuff
    if key == "id" then
        local old_taskdir = struct.tasks.path .. "/" .. env .. "/" .. id
        local new_taskdir = struct.tasks.path .. "/" .. env .. "/" .. val

        -- roachme: under development: taskunit does not rename unit dir
        if not utils.rename(old_taskdir, new_taskdir) then
            core.die(1, errmod.EETREN, id)
            return false
        end

        if not taskunit.ren(env, id, val) then
            core.die(1, errmod.EEUREN, id)
            return false
        end

        if id == currid then
            switch.id_delcurr()
            switch.id_addcurr(val)
        elseif id == previd then
            -- cuz there's no way to delete previous task id directly
            switch.env_swapspec()
            switch.env_delcurr()
        end
    end
    return true
end

---Switch to task.
---@param env string
---@param id string
---@return boolean
function core.id_switch(env, id)
    env = env or switch.env_getcurr().env

    -- check input values
    chkargs({ val = env, chk = true }, { val = id, chk = true })

    if not switch.id_addcurr(id) then
        core.die(1, errmod.get(errmod.errno), id)
    end
    return true
end

---Switch to previous task.
function core.id_prev()
    local envenv = switch.env_getcurr()
    local env = envenv.env
    local prev = envenv.prev

    if not env then
        core.die(1, errmod.EECUR, "env")
    elseif not prev then
        core.die(1, errmod.EIPRV, "id")
    elseif not switch.id_swapspec() then
        core.die(1, errmod.EISPRV, prev)
    end
    return true
end

---Get list of tasks.
---@param env string
---@return table
function core.id_list(env)
    local tasks = {}
    local currid = switch.id_getcurr()
    local previd = switch.id_getprev()
    env = env or switch.env_getcurr().env

    -- check input values
    chkargs({ val = env, chk = true }, { val = "", chk = false })

    local taskids = taskunit.list(env)
    for _, id in pairs(taskids) do
        local units = taskunit.get(env, id)
        local status
        -- roachme: gotta simplify it
        if id == currid then
            status = 0
        elseif id == previd then
            status = 1
        else
            status = 2
        end
        table.insert(tasks, { id = id, desc = units.desc, status = status })
    end
    return tasks
end

---Move task into another environment.
---@param env string
function core.id_move(env) end

---Get task units.
---@param env string
---@param id string
---@return table
function core.id_cat(env, id)
    local currenv = switch.env_getcurr()
    id = id or currenv.curr
    env = env or currenv.env

    -- check input values
    chkargs({ val = env, chk = true }, { val = id, chk = true })

    return taskunit.get(env, id)
end

core.prefix = prefix
core.dbdir = struct.dbdir.path

return core
