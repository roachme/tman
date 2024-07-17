local env = require("core.env")
local taskid = require("core.taskid")
local struct = require("plugin.struct")
local taskunit = require("core.taskunit")
local core = require("core.core")

--- Delete task.
local function tman_del()
    local id = arg[1]
    local envname = env.getcurr()

    if not envname then
        return core.die(1, "no current env", "env")
    elseif not env.exists(envname) then
        return core.die(1, "no such env", envname)
    end

    taskid.init(core.struct.ids.path)
    id = id or taskid.getcurr(envname)

    if not id then
        core.die(1, "no current task", "")
    elseif not taskid.exists(envname, id) then
        core.die(1, "no such task id", id)
    end

    io.write("Task: ")
    taskunit.cat(envname, id, "desc")
    io.write("Do you want to continue? [Yes/No] ")
    local confirm = io.read("*line")
    if confirm ~= "Yes" then
        print("deletion is cancelled")
        os.exit(1)
    end

    taskid.del(envname, id)
    taskunit.del(envname, id)

    -- if you delete current task, and are in current directory.
    -- delete task dir at the end, cuz it causes error for tman.sh
    struct.delete(envname, id)
    return 0
end

return tman_del
