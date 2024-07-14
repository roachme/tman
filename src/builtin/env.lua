local config = require("struct.config")
local git = require("plugin.git")
local env = require("core.env")
local shell = require("core.shell")
local core = require("core.core")
local taskid = require("core.taskid")
local utils = require("aux.utils")

env.init(core.struct.envs.path)

---Add new environment.
---@param envname string
local function _env_add(envname)
    local uconfig
    local path = core.struct.code.path

    if not envname then
        return core.die(1, "env name is required", "no envname")
    elseif env.exists(envname) then
        return core.die(1, "env name already exists", envname)
    end
    env.add(envname, utils.get_input("description"))
    shell.setcurr("")

    -- download repos.
    uconfig = config.uget(envname)
    for _, repo in pairs(uconfig.repos) do
        if not utils.access(path .. "/" .. repo.name) then
            if not git.repo_clone(repo.link, repo.name, path) then
                core.die(1, "could not download repo '%s'", "init", repo.name)
            end
        end
    end
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
    -- roachme: gotta structure it.
    -- update task as well
    taskid.init(core.struct.ids.path)
    local curr_id = taskid.getcurr(prevenv)
    if curr_id then
        shell.setcurr(utils.genname(prevenv, curr_id))
    else
        shell.setcurr("")
    end
end

---Switch to environment.
---@param envname string
local function _env_use(envname)
    if not env.exists(envname) then
        return core.die(1, "no such env name", envname)
    end
    env.setcurr(envname)

    taskid.init(core.struct.ids.path)
    local curr_id = taskid.getcurr(envname) or ""
    shell.setcurr(utils.genname(envname, curr_id))
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
