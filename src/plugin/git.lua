local gitlib = require("aux.gitlib")
local utils = require("aux.utils")

local git = {}

local repobase, repos, linkbase, dirbase

---Init git plugin stuff.
---@param _repos table
---@param _repobase string
---@param _linkbase string
---@param _dirbase string
function git.init(_repos, _repobase, _linkbase, _dirbase)
    repobase = _repobase
    repos = _repos
    linkbase = _linkbase
    dirbase = _dirbase
end

---Download repos and create symlinks.
---@param branch string
---@return boolean
function git.create(branch)
    -- create symlinks to repos
    for _, repo in pairs(repos) do
        local target = repobase .. "/" .. repo.name
        local linkname = linkbase .. "/" .. dirbase .. "/" .. repo.name
        utils.link(target, linkname, true)
    end

    -- create task branches in repos
    for _, repo in pairs(repos) do
        gitlib.branch_switch(repo.name, repo.branch, repobase)
        gitlib.branch_create(repo.name, branch, repobase)
        gitlib.branch_switch(repo.name, branch, repobase)
    end
    return true
end

---Delete task branches.
---@param branch string
---@return boolean
function git.delete(branch)
    for _, repo in pairs(repos) do
        gitlib.branch_delete(repo.name, branch, repobase)
    end
    return true
end

---Switch to task branch.
--Create a branch if does not exists.
---@param branch string
---@return boolean
function git.switch(branch)
    for _, repo in pairs(repos) do
        if gitlib.repo_isuncommited(repo.name, repobase) then
            return false
        elseif not gitlib.branch_exist(repo.name, branch, repobase) then
            if not gitlib.branch_create(repo.name, branch, repobase) then
                return false
            end
        end
    end

    for _, repo in pairs(repos) do
        gitlib.branch_switch(repo.name, branch, repobase)
    end
    return true
end

---Rename task branches.
---@param oldbranch string
---@param newbranch string
---@return boolean
function git.rename(oldbranch, newbranch)
    return true
end

---Update repos from remove git server.
---@param branch string
---@return boolean
function git.update_remote(branch)
    -- download repos if they don't exist.
    for _, repo in pairs(repos) do
        print("---")
        if not gitlib.repo_clone(repo.link, repo.name, repobase) then
            io.stderr:write(("could not download repo '%s'\n"):format(repo.name))
            return false
        end
    end

    -- make sure task has symlinks to repos.
    for _, repo in pairs(repos) do
        local target = repobase .. "/" .. repo.name
        local linkname = linkbase .. "/" .. dirbase .. "/" .. repo.name
        if not utils.access(linkname) then
            if not utils.link(target, linkname, true) then
                io.stderr:write(("repo %s: could not create symlink\n"):format(repo.name))
                return false
            end
        end
    end

    -- general check
    for _, repo in pairs(repos) do
        if gitlib.repo_isuncommited(repo.name, repobase) then
            io.stderr:write(("repo %s: has uncommited changes\n"):format(repo.name))
            return false
        elseif not gitlib.branch_switch(repo.name, repo.branch, repobase) then
            io.stderr:write(("repo %s: could not switch to default branch\n"):format(repo.name))
            return false
        elseif not gitlib.branch_exist(repo.name, branch, repobase) then
            if not gitlib.branch_create(repo.name, branch, repobase) then
                io.stderr:write(("repo %s: could not create task branch\n"):format(repo.name))
                return false
            end
        end
    end

    for _, repo in pairs(repos) do
        gitlib.branch_pullall(repo.name, repobase)
        gitlib.branch_switch(repo.name, branch, repobase)
        if not gitlib.branch_rebase(repo.name, repo.branch, repobase) then
            io.stderr:write(("repo %s: could not automatically rebase against default branch\n"):format(repo.name))
            return false
        end
    end
    return true
end

---Update repos from remove git server.
---@param branch string
---@return boolean
function git.update_local(branch)
    -- make sure task has symlinks to repos.
    for _, repo in pairs(repos) do
        local target = repobase .. "/" .. repo.name
        local linkname = linkbase .. "/" .. dirbase .. "/" .. repo.name
        if not utils.access(linkname) then
            if not utils.link(target, linkname, true) then
                io.stderr:write(("repo %s: could not create symlink\n"):format(repo.name))
                return false
            end
        end
    end

    -- general check
    for _, repo in pairs(repos) do
        if gitlib.repo_isuncommited(repo.name, repobase) then
            io.stderr:write(("repo %s: has uncommited changes\n"):format(repo.name))
            return false
        elseif not gitlib.branch_switch(repo.name, branch, repobase) then
            io.stderr:write(("repo %s: could not switch to task branch\n"):format(repo.name))
            return false
        end
    end

    for _, repo in pairs(repos) do
        gitlib.branch_switch(repo.name, branch, repobase)
        if not gitlib.branch_rebase(repo.name, repo.branch, repobase) then
            io.stderr:write(("repo %s: could not automatically rebase against default branch\n"):format(repo.name))
            return false
        end
    end
    return true
end

---Clone repos.
function git.clone()
    for _, repo in pairs(repos) do
        if not utils.access(repobase .. "/" .. repo.name) then
            if not gitlib.repo_clone(repo.link, repo.name, repobase) then
                return false
            end
        end
    end
    return true
end

return git
