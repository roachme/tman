local env = require("core.env")
local git = require("secondary.git")
local taskid = require("core.taskid")
local struct = require("secondary.struct")
local config = require("secondary.config")
local taskunit = require("core.taskunit")
local common = require("core.common")
local shell = require("aux.shell")
local utils = require("aux.utils")

--- Delete task.
local function tman_del()
    local id = arg[1]
    local envname = env.getcurr()

    if not envname then
        return common.die(1, "no current env", "env")
    elseif not env.exists(envname) then
        return common.die(1, "no such env", envname)
    end

    taskid.init(config.core.refs.ids)
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

    local path = config.aux.code
    local branch = taskunit.get(envname, id, "branch")
    if not branch then
        return common.die_atomic(id, "task unit file missing branch\n", id)
    end

    -- delete task id's branches.
    for _, repo in pairs(config.user.repos) do
        if git.repo_isuncommited(repo.name, path) then
            return common.die(1, "repo has uncommited changes\n", repo.name)
        end
    end
    for _, repo in pairs(config.user.repos) do
        git.branch_switch(repo.name, repo.branch, path)
        git.branch_delete(repo.name, branch, path)
    end

    taskid.del(envname, id)
    taskunit.del(envname, id)

    -- switch to new current task if exists.
    local curr = taskid.getcurr(envname)
    if curr then
        branch = taskunit.get(envname, curr, "branch")
        if not branch then
            return common.die(1, "task unit file missing branch\n", curr)
        end
        for _, repo in pairs(config.user.repos) do
            git.branch_switch(repo.name, branch, path)
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
