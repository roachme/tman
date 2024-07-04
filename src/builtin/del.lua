local env = require("core.env")
local git = require("secondary.git")
local taskid = require("core.taskid")
local struct = require("secondary.struct")
local config = require("secondary.config")
local taskunit = require("core.taskunit")
local common = require("core.common")

--- Delete task.
local function tman_del()
    local id = arg[1]
    local envname = env.getcurr()

    if not envname then
        return common.die(1, "no current env", "env")
    elseif not env.exists(envname) then
        return common.die(1, "no such env", envname)
    end

    taskid.init(config.core.ids)
    id = id or taskid.getcurr(envname)

    if not id then
        common.die(1, "no current task\n", "")
    elseif not taskid.exists(envname, id) then
        common.die(1, "no such task ID\n", id)
    end

    io.write("Task: ")
    taskunit.cat(envname, id, "desc")
    io.write("Do you want to continue? [Yes/No] ")
    local confirm = io.read("*line")
    if confirm ~= "Yes" then
        print("deletion is cancelled")
        os.exit(1)
    end

    if git.branch_isuncommited() then
        common.die(1, "repo has uncommited changes", "")
    end
    git.branch_delete(id)

    taskunit.del(envname, id)
    taskid.del(envname, id)

    -- roachme: make it pretty and easire to read.
    -- switch back to current task (if exists)
    local curr = taskid.getcurr(envname)
    if curr then
        git.branch_switch(curr)
    end

    -- delete task dir at the end, cuz it causes error for tman.sh
    struct.delete(common.genname(envname, id))
    return 0
end

return tman_del
