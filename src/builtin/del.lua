local git = require("secondary.git")
local taskid = require("core.taskid")
local struct = require("secondary.struct")
local taskunit = require("core.taskunit")
local common = require("core.common")

--- Delete task.
local function tman_del()
    local id = arg[1] or taskid.getcurr()

    if not id then
        common.die(1, "no current task\n", "")
    end
    if not taskid.exist(id) then
        common.die(1, "no such task ID\n", id)
    end

    io.write("Task: ")
    taskunit.cat(id, "desc")
    io.write("Do you want to continue? [Yes/No] ")
    local confirm = io.read("*line")
    if confirm ~= "Yes" then
        print("deletion is cancelled")
        os.exit(1)
    end

    if not git.branch_isuncommited() then
        common.die(1, "repo has uncommited changes", "")
    end
    git.branch_delete(id)
    taskunit.del(id)
    taskid.del(id)

    -- roachme: make it pretty and easire to read.
    -- switch back to current task (if exists)
    local curr = taskid.getcurr()
    if curr then
        git.branch_switch(curr)
    end

    -- delete task dir at the end, cuz it causes error for tman.sh
    struct.delete(id)
    return 0
end

return tman_del
