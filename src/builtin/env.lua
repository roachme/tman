local env = require("core.env")
local shell = require("aux.shell")
local common = require("core.common")
local taskid = require("core.taskid")
local utils = require("aux.utils")
local config = require("secondary.config")

env.init(config.core.refs.envs)

---Add new environment.
---@param envname string
local function _env_add(envname)
    if not envname then
        return common.die(1, "env name is required\n", "no envname")
    end
    if env.exists(envname) then
        return common.die(1, "such env name already exists\n", envname)
    end
    env.add(envname, utils.get_input("description"))
    shell.setcurr("")
end

---Delete an environment.
---@param envname string
local function _env_del(envname)
    if not envname then
        common.die(1, "no current environment\n", "env")
    elseif not env.exist(envname) then
        common.die(1, "no such environment\n", "env")
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
        return common.die(1, "no previous env\n", "env")
    end

    env.setcurr(prevenv)
    -- roachme: gotta structure it.
    -- update task as well
    taskid.init(config.core.refs.ids)
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
        return common.die(1, "no such env name\n", envname)
    end
    env.setcurr(envname)

    taskid.init(config.core.refs.ids)
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
        common.die(1, "no such env command\n", cmd)
    end
    return 0
end

return builtin_env
