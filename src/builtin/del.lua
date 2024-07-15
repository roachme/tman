local env = require("core.env")
local gitlib = require("aux.gitlib")
local taskid = require("core.taskid")
local struct = require("struct.struct")
local config = require("struct.config")
local taskunit = require("core.taskunit")
local core = require("core.core")
local shell = require("core.shell")
local utils = require("aux.utils")

--- Delete task.
local function tman_del()
    local id = arg[1]
    local uconfig
    local envname = env.getcurr()

    if not envname then
        return core.die(1, "no current env", "env")
    elseif not env.exists(envname) then
        return core.die(1, "no such env", envname)
    end

    uconfig = config.uget(envname)
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

    local path = core.struct.code.path
    local branch = taskunit.get(envname, id, "branch")
    if not branch then
        return core.die(id, "task unit file missing branch", id)
    end

    -- delete task id's branches.
    for _, repo in pairs(uconfig.repos) do
        if gitlib.repo_isuncommited(repo.name, path) then
            return core.die(1, "repo has uncommited changes", repo.name)
        end
    end
    for _, repo in pairs(uconfig.repos) do
        gitlib.branch_switch(repo.name, repo.branch, path)
        gitlib.branch_delete(repo.name, branch, path)
    end

    taskid.del(envname, id)
    taskunit.del(envname, id)

    -- switch to new current task if exists.
    local curr = taskid.getcurr(envname)
    if curr then
        branch = taskunit.get(envname, curr, "branch")
        if not branch then
            return core.die(1, "task unit file missing branch", curr)
        end
        for _, repo in pairs(uconfig.repos) do
            gitlib.branch_switch(repo.name, branch, path)
        end

        -- cache current task id
        shell.setcurr(utils.genname(envname, curr or ""))
    end

    -- if you delete current task, and are in current directory.
    -- delete task dir at the end, cuz it causes error for tman.sh
    struct.delete(envname, id)
    return 0
end

return tman_del
