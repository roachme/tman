local gitlib = require("aux.gitlib")
local utils = require("aux.utils")

local git = {}

local repobase, repos, linkbase, dirbase, commitpatt

---Init git plugin stuff.
---@param _repos table
---@param _repobase string
---@param _linkbase string
---@param _dirbase string
---@param _commitpatt string
function git.init(_repos, _repobase, _linkbase, _dirbase, _commitpatt)
    repobase = _repobase
    repos = _repos
    linkbase = _linkbase
    dirbase = _dirbase
    commitpatt = _commitpatt
end

---Download repos and create symlinks.
---@param id string
---@param branch string
---@return boolean
function git.create(id, branch)
    -- prepare git
    if dirbase ~= "." and id then
        local dirbase_path = linkbase .. "/" .. id .. "/" .. dirbase
        if not utils.mkdir(dirbase_path) then
            return false
        end
    end

    -- create symlinks to repos
    for _, repo in pairs(repos) do
        local target = repobase .. "/" .. repo.name
        local linkname = linkbase .. "/" .. id .. "/" .. dirbase .. "/" .. repo.name
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
---@param id string
---@param branch string
---@return boolean
function git.delete(id, branch)
    for _, repo in pairs(repos) do
        gitlib.branch_delete(repo.name, branch, repobase)
    end
    return true
end

---Switch to task branch.
--Create a branch if does not exists.
---@param id string
---@param branch string
---@return boolean
function git.switch(id, branch)
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
---@param id string
---@param oldbranch string
---@param newbranch string
---@return boolean
function git.rename(id, oldbranch, newbranch)
    return true
end

---Update repos from remove git server.
---@param id string
---@param branch string
---@return boolean
function git.update_local(id, branch)
    -- download repos if they don't exist.
    for _, repo in pairs(repos) do
        if not gitlib.repo_clone(repo.link, repo.name, repobase) then
            io.stderr:write(("could not download repo '%s'\n"):format(repo.name))
            return false
        end
    end

    -- make sure task has symlinks to repos.
    for _, repo in pairs(repos) do
        local target = repobase .. "/" .. repo.name
        local linkname = linkbase .. "/" .. id .. "/" .. dirbase .. "/" .. repo.name
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

---Update local repo with remote (git push).
---@param id string
---@param branch string
---@return boolean
function git.update_remote(id, branch)
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

---Create commit.
---@param id string
---@return boolean
function git.commit_create(id)
    for _, repo in pairs(repos) do
        gitlib.commit_create(repo.name, commitpatt, id, repobase)
    end
    return true
end

function git.commit_squash()
end

return git
