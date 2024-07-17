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
function git.rename()
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
