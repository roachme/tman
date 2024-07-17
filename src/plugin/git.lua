local core = require("core.core")
local gitlib = require("aux.gitlib")
local config = require("aux.config")
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

---Rename task branches.
function git.rename()
end

return git
