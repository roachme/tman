local env = require("core.env")
local git = require("plugin.git")
local config = require("struct.config")
local taskid = require("core.taskid")
local struct = require("struct.struct")
local taskunit = require("core.taskunit")
local core = require("core.core")
local getopt = require("posix.unistd").getopt

--- Synchronize task dir: structure, task status, remote repo.
-- With no options jump to task dir.
-- TODO: if wd util not supported then add its features here. Optioon `-w'.
local function tman_sync()
    local id
    local uconfig
    local optstr = "rst"
    local fremote, fstruct, ftask
    local last_index = 1
    local envname = env.getcurr()
    local path = core.struct.code.path

    for optopt, _, optind in getopt(arg, optstr) do
        if optopt == "?" then
            core.die(1, "unrecognized option", arg[optind - 1])
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
        return core.die(1, "no current env", "env")
    end

    uconfig = config.uget(envname)
    taskid.init(core.struct.ids.path)

    id = arg[last_index] or taskid.getcurr(envname)
    if not id then
        core.die(1, "no current task id", "")
    elseif not taskid.exists(envname, id) then
        core.die(1, "no such task id", id)
    end

    local branch = taskunit.get(envname, id, "branch")

    if fremote then
        print("sync: remote")
        -- check that repos are ready to create task branch.
        for _, repo in pairs(uconfig.repos) do
            if git.repo_isuncommited(repo.name, path) then
                return core.die(1, "repo has uncommited changes", repo.name)
            end
        end
        for _, repo in pairs(uconfig.repos) do
            git.branch_switch(repo.name, repo.branch, path)
            git.branch_pullall(repo.name, path)
            git.branch_switch(repo.name, branch, path)
            git.branch_rebase(repo.name, repo.branch, path)
        end
    end
    if fstruct then
        print("sync: struct")
        local active_repos = {}
        local errfmt = "repo not on task branch tho has uncommited changes"

        for _, repo in pairs(uconfig.repos) do
            if git.branch_on(repo.name, branch, path) then
                if git.repo_isuncommited(repo.name, path) then
                    table.insert(active_repos, repo.name)
                elseif
                    git.branch_ahead(repo.name, repo.branch, branch, path)
                then
                    table.insert(active_repos, repo.name)
                end
            elseif git.repo_isuncommited(repo.name, path) then
                return core.die(1, errfmt, repo.name)
            end
        end

        -- create task branch if needed
        for _, repo in pairs(uconfig.repos) do
            if not git.branch_exist(repo.name, branch, path) then
                git.branch_create(repo.name, branch, path)
                git.branch_switch(repo.name, branch, path)
            end
        end

        struct.create(envname, id)
        local repos = table.concat(active_repos, " ")
        taskunit.set(envname, id, "repos", repos)
    end
    if ftask then
        print("sync: task status: under development")
    end
    return 0
end

return tman_sync
