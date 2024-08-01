local taskenv = require("core.taskenv")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")

--- Switch to task.
local function tman_use()
    local envname
    local id
    local unit_dir = core.struct.units.path
    local task_dir = core.struct.tasks.path
    -- roachme: can't use help option cuz tman.sh fails.

    -- system dependant (fatal): load core modules
    if not taskenv.init(core.struct.envs.path) then
        return core.die(1, "could not init module taskenv", "fatal")
    elseif not taskid.init(core.struct.ids.path, core.struct.curr.path) then
        return core.die(1, "could not init module taskid", "fatal")
    elseif not taskunit.init(unit_dir, task_dir) then
        return core.die(1, "could not init module taskunit", "fatal")
    end

    -- check: user dependant: sentinel guards
    envname = arg[2] or taskenv.getcurr()
    if not envname then
        return core.die(1, "no current env", "envname")
    elseif not taskenv.check(envname) then
        return core.die(1, "illegal environment name value", envname)
    elseif not taskenv.exist(envname) then
        return core.die(1, "no such environment name", envname)
    end

    id = arg[1]
    if not id then
        return core.die(1, "task id required", "")
    elseif not taskid.exist(envname, id) then
        return core.die(1, "task id does not exist", id)
    end

    -- it's the same id. Do nothing.
    if id and id == taskid.getcurr(envname) then
        return 0
    end
    taskid.setcurr(envname, id)
    return 0
end

return tman_use
