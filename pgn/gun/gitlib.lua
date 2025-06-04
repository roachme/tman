local utils = require("utils")

local git = {}

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

---Check if branch has commits ahead default.
---@param reponame string
---@param defbranch string
---@param taskbranch string
---@param path string | nil
---@return boolean
function git.branch_ahead(reponame, defbranch, taskbranch, path)
    --[[
    roachme:FIXME
        if a user has never switched to default branch, then command below
        causes problem.
    Error message:
        fatal: ambiguous argument 'develop..bugfix/test2_Test_task_2_20240712':
        unknown revision or path not in the working tree.
        Use '--' to separate paths from revisions, like this:
        'git <command> [<revision>...] -- [<file>...]'
    ]]
    path = path or "."
    local fmt = "git -C %s/%s rev-list --count --right-only %s..%s"
    local cmd = string.format(fmt, path, reponame, defbranch, taskbranch)
    local f = io.popen(cmd)
    if not f then
        return false
    end
    local count = tonumber(f:read("*a"))
    f:close()
    return count > 0 and true or false
end

---Clone a repo.
---@param link string
---@param reponame string
---@param path string | nil
---@return boolean
function git.repo_clone(link, reponame, path)
    path = path or "."

    if utils.access(path .. "/" .. reponame) then
        return true
    end

    local fmt = "git -C %s clone --quiet %s %s 2>/dev/null"
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

local function generate_commit_msg(id, commitpatt)
    local commitmsg = commitpatt
    local parts = { "ID", "PART", "MSG" }
    local act = { id, "MYPART", "hello there" }

    for i = 1, 3 do
        commitmsg = string.gsub(commitmsg, parts[i], act[i])
    end
    print("commitmsg", commitmsg)
    return commitmsg
end

---Create commit.
---@param reponame string
---@param desc string
---@param path string | nil
function git.commit_create(reponame, desc, path)
    path = path or "."
    local fmt = "git -C %s/%s add ."
    local cmd = string.format(fmt, path, reponame)
    utils.exec(cmd)

    fmt = "git -C %s/%s commit -m '%s'"
    cmd = string.format(fmt, path, reponame, desc)
    utils.exec(cmd)
end

return git
