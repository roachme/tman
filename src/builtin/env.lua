local env = require("core.env")
local core = require("core.core")
local taskid = require("core.taskid")
local utils = require("aux.utils")
local plugin = require("plugin")

env.init(core.struct.envs.path)
taskid.init(core.struct.ids.path)

---Add new environment.
---@param envname string
local function _env_add(envname)
    if not envname then
        return core.die(1, "env name is required", "no envname")
    elseif env.exists(envname) then
        return core.die(1, "env name already exists", envname)
    end
    env.add(envname, utils.get_input("description"))
    taskid.updcurr(envname)

    -- create env directory
    if not utils.mkdir(core.struct.tasks.path .. "/" .. envname) then
        return core.die(1, "could not create environment directory", "envdir")
    end


    -- roachme: no task id for now. might cause problems in function below.
    if not plugin.init(envname, nil) then
        return core.die(1, "could not init plugins", "plugin")
    end


    -- plugin: struct
    -- create env directory
    -- code goes here..

    -- plugin: git
    -- download repos
    plugin.git.clone()
    return 0
end

---Delete an environment.
---@param envname string
local function _env_del(envname)
    if not envname then
        core.die(1, "no current environment", "env")
    elseif not env.exist(envname) then
        core.die(1, "no such environment", "env")
    end

    io.write("Do you want to continue? [Yes/No] ")
    local confirm = io.read("*line")
    if confirm ~= "Yes" then
        print("deletion is cancelled")
        os.exit(1)
    end
    -- roachme: TODO
    -- delete taskid, taskunit and struct
    env.del(envname)
end

---Switch to previous environment.
---@param prevenv string?
local function _env_prev(prevenv)
    if not prevenv then
        return core.die(1, "no previous env", "env")
    end

    env.setcurr(prevenv)
    taskid.updcurr(prevenv)
end

---Switch to environment.
---@param envname string
local function _env_use(envname)
    if not env.exists(envname) then
        return core.die(1, "no such env name", envname)
    end
    env.setcurr(envname)
    taskid.updcurr(envname)
end

--- Define or display task environments.
local function builtin_env()
    local cmd = arg[1] or "list"
    local envname = arg[2]

    if cmd == "add" then
        _env_add(envname)
    elseif cmd == "del" then
        _env_del(envname or env.getcurr())
    elseif cmd == "list" then
        env.list()
    elseif cmd == "prev" then
        _env_prev(env.getprev())
    elseif cmd == "use" then
        _env_use(envname)
    else
        core.die(1, "no such env command", cmd)
    end
    return 0
end

return builtin_env
