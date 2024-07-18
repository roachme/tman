local taskenv = require("core.env")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local utils = require("aux.utils")

--- Delete task.
local function builtin_del()
    local id = arg[1]
    local envname
    local unit_dir = core.struct.units.path
    local task_dir = core.struct.tasks.path

    -- system dependant (fatal): load core modules
    if not taskenv.init(core.struct.envs.path) then
        return core.die(1, "could not init module taskenv", "fatal")
    elseif not taskid.init(core.struct.ids.path) then
        return core.die(1, "could not init module taskid", "fatal")
    elseif not taskunit.init(unit_dir, task_dir) then
        return core.die(1, "could not init module taskunit", "fatal")
    end

    -- check: user dependant: sentinel guards
    envname = taskenv.getcurr()
    if not envname then
        return core.die(1, "no current environment", "env")
    elseif not taskenv.check(envname) then
        return core.die(1, "illegal environment name value", envname)
    elseif not taskenv.exist(envname) then
        return core.die(1, "no such environment", envname)
    end

    id = arg[1] or taskid.getcurr(envname)
    if not id then
        return core.die(1, "no current task id", "")
    elseif not taskid.check(id) then
        return core.die(1, "illegal id value", id)
    elseif not taskid.exist(envname, id) then
        return core.die(1, "no such task id", id)
    end

    io.write("Task: ")
    taskunit.cat(envname, id, "desc")
    io.write("Do you want to continue? [Yes/No] ")
    local confirm = io.read("*line")
    if confirm ~= "Yes" then
        print("deletion is cancelled")
        os.exit(1)
    end

    -- delete task from database.
    local taskdir = core.struct.tasks.path .. "/" .. envname .. "/" .. id
    if not taskid.del(envname, id) then
        return core.die(1, "fatal: could not delete task id", "taskid")
    elseif not taskunit.del(envname, id) then
        return core.die(1, "fatal: could not delete task unit", "taskunit")
    elseif not utils.rm(taskdir) then
        return core.die(1, "fatal: could not delete task dir", "dir")
    end

    return 0
end

return builtin_del
