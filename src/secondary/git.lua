local utils = require("aux.utils")
local commit = require("commit")

local git = {}

---Create commit.
---@param reponame string
---@param path string | nil
function git.commit_create(reponame, path)
    path = path or "."
    local fmt = "git -C %s/%s git add ."
    local cmd = string.format(fmt, path, reponame)
    utils.exec(cmd)
    return commit.create()
end

--[[
---Squash task commits.
---@param reponame string
---@param path string | nil
function git.commit_squash(reponame, path)
    path = path or "."
end
]]

---Create branch.
---@param reponame string
---@param branch string
---@param path string | nil
---@return boolean
function git.branch_create(reponame, branch, path)
    path = path or "."
    local fmt = "git -C %s/%s branch %s 2>/dev/null"
    local cmd = string.format(fmt, path, reponame, branch)
    return utils.exec(cmd)
end

---Delete branch.
---@param reponame string
---@param branch string
---@param path string | nil
---@return boolean
function git.branch_delete(reponame, branch, path)
    path = path or "."
    local fmt = "git -C %s/%s branch -q -D %s"
    local cmd = string.format(fmt, path, reponame, branch)
    return utils.exec(cmd)
end

---Switch to branch.
---@param reponame string
---@param branch string
---@param path string | nil
---@return boolean
function git.branch_switch(reponame, branch, path)
    path = path or "."
    local fmt = "git -C %s/%s checkout -q %s 2>/dev/null"
    local cmd = string.format(fmt, path, reponame, branch)
    return utils.exec(cmd)
end

---Rename branch.
---@param reponame string
---@param oldbranch string
---@param newbranch string
---@param path string | nil
---@return boolean
function git.branch_rename(reponame, oldbranch, newbranch, path)
    path = path or "."
    local fmt = "git -C %s/%s branch -m %s %s"
    local cmd = string.format(fmt, path, reponame, oldbranch, newbranch)
    return utils.exec(cmd)
end

---Rebase branch.
---@param reponame string
---@param branch string
---@param path string | nil
---@return boolean
function git.branch_rebase(reponame, branch, path)
    path = path or "."
    local fmt = "git -C %s/%s rebase -q %s"
    local cmd = string.format(fmt, path, reponame, branch)
    return utils.exec(cmd)
end

---Merge branch.
---@param reponame string
---@param branch string
---@param path string | nil
---@return boolean
function git.branch_merge(reponame, branch, path)
    path = path or "."
    local fmt = "git -C %s/%s merge %s"
    local cmd = string.format(fmt, path, reponame, branch)
    return utils.exec(cmd)
end

---Pull branch.
---@param reponame string
---@param remote string
---@param path string | nil
---@param branch string
---@return boolean
function git.branch_pull(reponame, remote, branch, path)
    path = path or "."
    local fmt = "git -C %s/%s pull -q %s %s"
    local cmd = string.format(fmt, path, reponame, remote, branch)
    return utils.exec(cmd)
end

---Pull branch.
---@param reponame string
---@param path string | nil
---@return boolean
function git.branch_pullall(reponame, path)
    path = path or "."
    local fmt = "git -C %s/%s pull -q"
    local cmd = string.format(fmt, path, reponame)
    return utils.exec(cmd)
end

---Push branch.
---@param reponame string
---@param remote string
---@param branch string
---@param path string | nil
---@return boolean
function git.branch_push(reponame, remote, branch, path)
    path = path or "."
    local fmt = "git -C %s/%s push %s %s"
    local cmd = string.format(fmt, path, reponame, remote, branch)
    return utils.exec(cmd)
end

---Check that branch exists.
---@param reponame string
---@param branch string
---@param path string | nil
---@return boolean
function git.branch_exist(reponame, branch, path)
    path = path or "."
    local fmt = "git -C %s/%s show-ref --quiet refs/heads/%s"
    local cmd = string.format(fmt, path, reponame, branch)
    return utils.exec(cmd)
end

---Check that on needed branch.
---@param reponame string
---@param branch string
---@param path string | nil
---@return boolean
function git.branch_on(reponame, branch, path)
    --local fmt = "git -C %s/%s branch --show-current"
    local fmt = "git -C %s/%s rev-parse --abbrev-ref HEAD"
    local cmd = string.format(fmt, path, reponame)
    local f = io.popen(cmd)
    if not f then
        return false
    end
    local out = f:read("*a")
    f:close()

    out = string.gsub(out, "%s", "") -- trim newline
    if out == branch then
        return true
    end
    return false
end

---Clone a repo.
---@param link string
---@param reponame string
---@param path string | nil
---@return boolean
function git.repo_clone(link, reponame, path)
    path = path or "."
    local fmt = "git -C %s clone --quiet %s %s"
    local cmd = string.format(fmt, path, link, reponame)
    return utils.exec(cmd)
end

---Check that repo has uncommited changes.
---@param reponame string
---@param path string | nil
---@return boolean
function git.repo_isuncommited(reponame, path)
    path = path or "."
    -- roachme: find a better command for this not to use popen()
    local fmt = "git -C %s/%s status --untracked-files --short"
    local cmd = string.format(fmt, path, reponame)
    local f = io.popen(cmd)
    if not f then
        return false
    end
    local out = f:read("*a")
    f:close()
    if out == "" then
        return false
    end
    return true
end

return git
