local env = require("core.env")
local git = require("secondary.git")
local config = require("secondary.config")
local taskid = require("core.taskid")
local struct = require("secondary.struct")
local taskunit = require("core.taskunit")
local common = require("core.common")
local getopt = require("posix.unistd").getopt

--- Synchronize task dir: structure, task status, remote repo.
-- With no options jump to task dir.
-- TODO: if wd util not supported then add its features here. Optioon `-w'.
local function tman_sync()
    local id
    local optstr = "rst"
    local fremote, fstruct, ftask
    local last_index = 1
    local envname = env.getcurr()
    local path = config.aux.code

    for optopt, _, optind in getopt(arg, optstr) do
        if optopt == "?" then
            common.die(1, "unrecognized option\n", arg[optind - 1])
        end

        last_index = optind
        if optopt == "r" then
            fremote = true
        elseif optopt == "s" then
            fstruct = true
        elseif optopt == "t" then
            ftask = true
        end
    end

    if not envname then
        return common.die(1, "no current env", "env")
    end

    taskid.init(config.core.refs.ids)

    id = arg[last_index] or taskid.getcurr(envname)
    if not id then
        common.die(1, "no current task id\n", "")
    elseif not taskid.exists(envname, id) then
        common.die(1, "no such task id\n", id)
    end

    local branch = taskunit.get(envname, id, "branch")

    if fstruct then
        print("sync: struct")
        local active_repos = {}
        local errfmt = "repo not no task branch tho has uncommited changes\n"

        -- roach:BUG: It's gotta update list of active repos, but can't if
        -- they have uncommited changes. BUT it should be able.
        for _, repo in pairs(config.user.repos) do
            if git.branch_on(repo.name, branch, path) then
                if git.repo_isuncommited(repo.name, path) then
                    table.insert(active_repos, repo.name)
                elseif git.branch_ahead(repo.name, repo.branch, branch, path) then
                    -- branch has commits ahead of default branch
                    table.insert(active_repos, repo.name)
                end
            else
                if git.repo_isuncommited(repo.name, path) then
                    return common.die(1, errfmt, repo.name)
                else
                    git.branch_switch(repo.name, branch, path)
                end
            end
        end

        for _, repo in pairs(config.user.repos) do
            git.branch_create(repo.name, branch, path)
            git.branch_switch(repo.name, branch, path)
        end

        struct.create(envname, id)
        local repos = table.concat(active_repos, " ")
        taskunit.set(envname, id, "repos", repos)
    end
    if fremote then
        print("sync: remote")
        -- check that repos are ready to create task branch.
        for _, repo in pairs(config.user.repos) do
            if git.repo_isuncommited(repo.name, path) then
                return common.die(1, "repo has uncommited changes\n", repo.name)
            end
        end
        for _, repo in pairs(config.user.repos) do
            git.branch_switch(repo.name, repo.branch, path)
            git.branch_pullall(repo.name, path)
            git.branch_switch(repo.name, branch, path)
            git.branch_rebase(repo.name, repo.branch, path)
        end
    end
    if ftask then
        print("sync: task status: under development")
    end
    return 0
end

return tman_sync
