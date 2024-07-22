local core = require("core.core")
local plugin = require("core.plugin")
local gitlib = require("aux.gitlib")
local utils = require("aux.utils")

local git = {}
local plugin_name = "git"
local codedir = plugin.prefix .. "code/"

---Clone git repositories.
---@param envname string
---@return boolean
function git.clone(envname)
    local repos = plugin.get_uconfig(envname).git.repos

    if not utils.mkdir(codedir) then
        core.die(1, "could not create code directory", "git")
        return false
    end

    for _, repo in pairs(repos) do
        if not gitlib.repo_clone(repo.link, repo.name, codedir) then
            core.die(1, "could not download repo", repo.name)
            return false
        end
    end
    return true
end

---Symlink git repos to task dir.
---@param envname string
---@param id string
---@return boolean
function git.symlink_create(envname, id)
    local uconfig = plugin.get_uconfig(envname).git
    local repos = uconfig.repos
    local dirbase = uconfig.dirbase
    local taskdir = plugin.prefix .. "tasks/" .. envname .. "/" .. id .. "/"

    -- make sure repos directory exists.
    if not utils.access(taskdir .. "/" .. dirbase) then
        if not utils.mkdir(taskdir .. "/" .. dirbase) then
            core.die(1, "could not create repo directory", id)
            return false
        end
    end

    for _, repo in pairs(repos) do
        local target = codedir .. repo.name
        local linkname = taskdir .. dirbase .. "/" .. repo.name

        if not utils.access(target) then
            core.die(1, "repo not downloaded", repo.name)
            return false
        end
        if not utils.access(linkname) then
            if not utils.link(target, linkname, true) then
                core.die(1, "could not create symlink", repo.name)
                return false
            end
        end
    end
    return true
end

local gitunits = {}

---Load plugin git units.
---@param envname string
---@param uniqid string
---@return boolean
function git.loadunits(envname, uniqid)
    local fname = plugin.prefix .. ".tman/" .. "plugin/" .. envname .. "/" .. uniqid
    local f = io.open(fname)

    if not f then
        return true
    end

    for line in f:lines() do
        local pgnname, key, value = string.match(line, "(.*) (.*) (.*)")
        if pgnname == plugin_name then
            gitunits[key] = value
        end
    end
    return f:close() == true
end

---String separator.
---@param inputstr string
---@param sep string | nil
---@return table
local function pattsplit(inputstr, sep)
    local res = {}

    if sep == nil then
        sep = "%s"
    end
    for str in string.gmatch(inputstr, "([^" .. sep .. "]+)") do
        table.insert(res, str)
    end
    return res
end

local function build_branch_by_pattern(branchpatt, units)
    local separators = "/_-"
    local sepcomponents = pattsplit(branchpatt, separators)

    for _, item in pairs(sepcomponents) do
        local uitem = units[string.lower(item)]

        -- roachme: HOTFIX: desc: replace whitespace with undrescore
        if item == "DESC" then
            uitem = string.gsub(uitem, " ", "_")
        end
        branchpatt = string.gsub(branchpatt, item, uitem)
    end
    return branchpatt
end

function git.saveunits(envname, uniqid)
    local fname = plugin.prefix .. ".tman/" .. "plugin/" .. envname .. "/" .. uniqid
    local f = io.open(fname, "w")

    if not f then
        core.die(1, "could not save plugin git units", uniqid)
        return false
    end

    for key, value in pairs(gitunits) do
        local result = ("%s %s %s"):format(plugin_name, key, value)
        f:write(result .. "\n")
    end
    return f:close() == true
end

function git.getunits(key)
    return gitunits[key]
end

function git.setunits()
end

---Create git branch for task.
---@param envname string
---@param id string
---@return boolean
function git.branch_create(envname, id)
    local uconfig = plugin.get_uconfig(envname).git
    local repos = uconfig.repos
    local taskunits = plugin.getunits(envname, id)

    if not taskunits.uniqid then
        core.die(1, "fatal: task has no uniqid", id)
        return false
    end

    git.loadunits(envname, taskunits.uniqid)
    local task_branch = build_branch_by_pattern(uconfig.branchpatt, taskunits)
    local pgn_branch = git.getunits("branch")

    -- create new/fresh task branch
    if not pgn_branch  then
        print("-- create new task branch")
        for _, repo in pairs(repos) do
            if gitlib.repo_isuncommited(repo.name, codedir) then
                core.die(1, "repo has uncommited changes", repo.name)
                return false
            end
            if not gitlib.branch_switch(repo.name, repo.branch, codedir) then
                core.die(1, "no repo default branch '%s'", repo.name, repo.branch)
                return false
            end
            if not gitlib.branch_exist(repo.name, task_branch, codedir) then
                gitlib.branch_create(repo.name, task_branch, codedir)
            end
            gitlib.branch_switch(repo.name, task_branch, codedir)
        end
        gitunits.branch = task_branch

    else
        -- update plugin branch with existing task branch
        for _, repo in pairs(repos) do
            if gitlib.repo_isuncommited(repo.name, codedir) then
                core.die(1, "repo has uncommited changes", repo.name)
                return false
            end
            if not gitlib.branch_on(repo.name, pgn_branch, codedir) then
                if not gitlib.branch_exist(repo.name, pgn_branch, codedir) then
                    if not gitlib.branch_create(repo.name, pgn_branch, codedir) then
                        core.die(1, "could not create plugin branch", repo.name, pgn_branch)
                        return false
                    end
                end
                if not gitlib.branch_switch(repo.name, pgn_branch, codedir) then
                    core.die(1, "no repo plugin branch '%s'", repo.name, pgn_branch)
                    return false
                end
            end
            if not gitlib.branch_rename(repo.name, pgn_branch, task_branch, codedir) then
                core.die(1, "could not rename task branch", repo.name)
                return false
            end
            if not gitlib.branch_switch(repo.name, task_branch, codedir) then
                core.die(1, "could not switch to task branch", repo.name)
                return false
            end
        end
        gitunits.branch = task_branch
    end

    git.saveunits(envname, taskunits.uniqid)
    return true
end

function git.sync(envname, id)
    git.clone(envname)
    git.symlink_create(envname, id)
    git.branch_create(envname, id)
end

function git.cleanup()
end

return git
