local plugin = require("core.plugin")
local gitlib = require("aux.gitlib")
local utils = require("aux.utils")

local git = {}

local prefix_codebase -- = plugin.prefix .. "/" .. "code"
local file_plugin_units
local repobase, repos, linkbase, dirbase, commitpatt, branchpatt



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

---Form branch according to pattern.
---@return string
local function build_branch(units)
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

---Init git plugin stuff.
---@param _repos table
---@param _repobase string
---@param _linkbase string
---@param _dirbase string
---@param _commitpatt string
function git.init(_repos, _repobase, _linkbase, _dirbase, _commitpatt, _branchpatt)
    repobase = _repobase
    repos = _repos
    linkbase = _linkbase
    dirbase = _dirbase
    commitpatt = _commitpatt
    branchpatt = _branchpatt
end

local function save_metadata(items, repos, branch)
end


local function git_mkdir_codebase()
    print("git.sync: create plugin codebase directory if needed")
    if utils.access(prefix_codebase) then
        return true
    end
    return utils.mkdir(prefix_codebase)
end

local function git_get_plugin_units(pgnname)
    print("git.sync: read plugin units")
    local f = io.open(file_plugin_units)
    if not f then
        print("file_plugin_units: err", file_plugin_units)
        return {}
    end

    local output = { }
    for line in f:lines() do
        local plugin_name, key, value = string.match(line, "(.*) (.*) (.*)")
        if plugin_name == pgnname then
            output[key] = value
        end
    end
    return output
end

local function bulid_branch_by_pattern(_branchpatt, units)
    print("bulid_branch_by_pattern: branchpatt", _branchpatt)
    print("bulid_branch_by_pattern: branchpatt", units)
    local separators = "/_-"
    local sepcomponents = pattsplit(_branchpatt, separators)

    for _, item in pairs(sepcomponents) do
        local uitem = units[string.lower(item)]

        -- roachme: HOTFIX: desc: replace whitespace with undrescore
        if item == "DESC" then
            uitem = string.gsub(uitem, " ", "_")
        end
        _branchpatt = string.gsub(_branchpatt, item, uitem)
    end
    return _branchpatt
end

local function git_update_branch_names(branch_orig, branch_pgn)
    print("git.sync: compare git plugin branch with task units")
    print("git.sync: update branch name if needed")
    print("branch_orig", branch_orig)
    print("branch_pgn", branch_pgn)

    for _, repo in pairs(plugin.uconfig.git.repos) do
        print("git_update_branch_names: repo", repo.name)
        if gitlib.branch_exist(repo.name, branch_pgn, prefix_codebase)
           and gitlib.branch_exist(repo.name, branch_orig, prefix_codebase) then
            print("--- rename plugin existing branch")
            gitlib.branch_switch(repo.name, branch_orig, prefix_codebase)
            gitlib.branch_rename(repo.name, branch_pgn, branch_orig, prefix_codebase)

        elseif gitlib.branch_exist(repo.name, branch_pgn, prefix_codebase)
           and gitlib.branch_exist(repo.name, branch_orig, prefix_codebase)
           and branch_orig ~= branch_pgn then
            print("--- rename existing branch")
            gitlib.branch_switch(repo.name, branch_orig, prefix_codebase)
            gitlib.branch_rename(repo.name, branch_pgn, branch_orig, prefix_codebase)

        else
            -- create task branch (branch off the repo default branch)
            print("--- create a new branch")
            gitlib.branch_switch(repo.name, repo.branch, prefix_codebase)
            gitlib.branch_create(repo.name, branch_orig, prefix_codebase)
            gitlib.branch_switch(repo.name, branch_orig, prefix_codebase)
        end
    end

    -- update plugin branch with origin task branch
    -- code goes here...

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

--[[
lsync       - update branch names and symlinks
rsync       - update branch from remote git repository
cleanup     - delete task branches
]]

function git.sync(envname, id)
    print("git.sync: init plugin")
    if not plugin.init(envname, id) then
        return false
    end

    prefix_codebase = plugin.prefix .. "/" .. "code"
    file_plugin_units = plugin.prefix .. ".tman/" .. "plugin/" .. envname .. "/" .. id

    if not utils.mkdir(prefix_codebase) then
        io.stderr:write("ERROR: could not create plugin git codebase dir\n")
        os.exit(1)
    end

    if not git.clone() then
        io.stderr:write("ERROR: git_clone_repos: error")
        os.exit(1)
    end

    local branch_orig = bulid_branch_by_pattern(plugin.uconfig.git.branchpatt, plugin.units)
    local branch_pgn = git_get_plugin_units("git").branch

    if not git_update_branch_names(branch_orig, branch_pgn) then
        io.stderr:write("ERROR: git_update_branch_names: error")
        os.exit(1)
    end

    print("- git.sync")
end

---Delete task branches.
---@param id string
---@param branch string
---@return boolean
function git.delete(id, branch)
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

---Create commit.
---@param id string
---@return boolean
function git.commit_create(id)
    for _, repo in pairs(repos) do
        gitlib.commit_create(repo.name, commitpatt, id, repobase)
    end
    return true
end

function git.commit_squash() end

return git
