--- Git wrapper.
-- @module Git

local env = require("core.env")
local config = require("secondary.config")
local taskunit = require("core.taskunit")
local utils = require("aux.utils")

local git = {}

local repos = config.user.repos

-- local git = "git -C %s " -- roachme: how to use it in here
local gcheckout = "git -C %s checkout --quiet %s"
local gcheckoutb = "git -C %s checkout --quiet -b %s 2>/dev/null"
local gpull = "git -C %s pull --quiet origin %s"
local gpull_generic = "git -C %s pull --quiet"
local gbranchD = "git -C %s branch --quiet -D %s"
local gbranchm = "git -C %s branch --quiet -m %s"
local gbranchmrg = "git -C %s branch --merged %s | grep -q %s"
local gbranchprune = "git -C %s remote update origin --prune 1>/dev/null"
local gdiff_commits = "git -C %s diff --quiet --exit-code %s %s"
local grebase = "git -C %s rebase --quiet %s 2> /dev/null > /dev/null"
local grebaseabort = "git -C %s rebase --abort"

-- githelper functions: start --

--- Get changed part of the code.
local function changed_files(reponame)
    local files = {}
    local repopath = config.aux.code .. reponame
    local cmd = ("git -C %s diff --name-only"):format(repopath)
    local fprog = io.popen(cmd)

    if not fprog then
        print("git: could not run the shell program")
        os.exit(1)
    end

    for file in fprog:lines() do
        table.insert(files, file)
    end
    return files
end

--[[

commit message pattern
 [<ticket ID>] <изменяемая часть>: <краткое описание>
]]

local function get_dirname(ftab)
    local dirname
    local dirnames = {}

    for _, fname in pairs(ftab) do
        local tmp = string.match(fname, "(.*)/")
        if tmp then
            table.insert(dirnames, tmp)
        else
            -- if file in the root of repo
            table.insert(dirnames, fname)
        end
    end

    -- traverse dirname of all files.
    -- if they're the same, return dirname
    -- else return nil, and reponame with will be dirname
    dirname = dirnames[1]
    for _, dname in pairs(dirnames) do
        if dirname ~= dname then
            return nil
        end
    end
    return dirname
end

local function changed_part(reponame)
    local files = changed_files(reponame)
    local dirname = get_dirname(files) or reponame
    return dirname
end

-- githelper functions: start --

-- Private functions: end --

--- Check commit message according to patterns.
-- Under development.
-- @param msg commit message
local function commit_check(msg)
    if msg then
        return true
    end
    return true
end

local function _repo_exists(reponame)
    local repopath = config.aux.code .. reponame

    if utils.access(repopath) then
        return true
    end
    return false
end

--- Check that task branch exists.
local function _branch_exists(repopath, branchname)
    local gitcmd = "git -C %s "
    local cmd_branch_exists = gitcmd .. "show-ref --quiet refs/heads/%s"
    local cmd = cmd_branch_exists:format(repopath, branchname)
    local retcode = utils.exec(cmd)
    if not retcode then
        return false
    end
    return true
end

--- Check whether repo has uncommited changes.
-- @param reponame repo name
-- @return has uncommited changes - true
-- @return no uncommited changes -  false
local function is_repo_clean(reponame)
    -- roachme: gotta take argument for task branch, right?
    local repopath = config.aux.code .. reponame
    local cmd = "git -C %s diff --quiet --exit-code"
    cmd = string.format(cmd, repopath)
    local ret = utils.exec(cmd)
    return ret == 0 or ret == true
end


---Check that repo has uncommited changes.
---@param reponame string
---@param repopath string | nil
---@return boolean
function git.repo_isuncommited(reponame, repopath)
    repopath = repopath or "."
    -- roachme: find a better command for this not to use popen()
    local fmt = "git -C %s/%s status --untracked-files --short"
    local cmd = string.format(fmt, repopath, reponame)
    local f = io.popen(cmd)
    if not f then
        return false
    end
    local out = f:read("*a")
    f:close()
    return out == "" and true or false
end
function git.branch_isuncommited()
    -- roachme: depricated.
    for _, repo in pairs(repos) do
        local ret = is_repo_clean(repo.name)
        if ret then
            return false
        end
    end
    return true
end


-- Private functions: end --

-- Public functions: start --

--- Switch to repo default branch.
-- @treturn bool true on success, otherwise false
function git.branch_default()
    for _, repo in pairs(repos) do
        local repopath = config.aux.code .. repo.name
        utils.exec(gcheckout:format(repopath, repo.branch))
    end
    return true
end

--- Git pull command.
-- @param all true pull all branches, otherwise only default branch
function git.branch_update(all)
    for _, repo in pairs(repos) do
        local repopath = config.aux.code .. repo.name
        utils.exec(gcheckout:format(repopath, repo.branch))
        if all then
            utils.exec(gpull_generic:format(repopath))
        else
            utils.exec(gpull:format(repopath, repo.branch))
        end
    end
end

--- Rebase task branch against default.
function git.branch_rebase()
    for _, repo in pairs(repos) do
        local repopath = config.aux.code .. repo.name
        if not utils.exec(grebase:format(repopath, repo.branch)) then
            local errmsg = "repo '%s': rebase conflic. Resolve it manually.\n"
            io.stderr:write((errmsg):format(repo.name))
            utils.exec(grebaseabort:format(repopath))
        end
    end
end

---Create branch.
---@param reponame string
---@param branch string
---@param path string | nil
---@return boolean
function git.branch_create(reponame, branch, path)
    path = path or "."
    local fmt = "git -C %s/%s branch %s"
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
    local fmt = "git -C %s/%s checkout -q %s"
    local cmd = string.format(fmt, path, reponame, branch)
    return utils.exec(cmd)
end

--- Rename task branch.
-- @param id task ID
-- @return true on success, otherwise false
function git.branch_rename(id)
    local newbranch = taskunit.get(env.getcurr(), id, "branch")

    for _, repo in pairs(repos) do
        local repopath = config.aux.code .. repo.name
        utils.exec(gbranchm:format(repopath, newbranch))
    end
    return 0
end

--- Delete task branch.
-- @param id task ID
-- @return true on success, otherwise false
function git.branch_delete(id)
    local branch = taskunit.get(env.getcurr(), id, "branch")

    for _, repo in pairs(repos) do
        local repopath = config.aux.code .. repo.name
        utils.exec(gcheckout:format(repopath, repo.branch))

        -- if there's branch in task unit file then delete it.
        if branch then
            utils.exec(gbranchD:format(repopath, branch))
        end
    end
    return true
end

--- Check that all task's repo branches are merged into the default one.
-- roachme: let a user know if no task commits presented.
-- @return task branch's merged - true
-- @return task branch's not merged - false
function git.branch_merged(id)
    local retcode = true
    local branch = taskunit.get(env.getcurr(), id, "branch")

    --  roachme: doesn't work if merge conflic with default branch.
    --  which happens quite often.

    for _, repo in pairs(repos) do
        local repopath = config.aux.code .. repo.name
        local cmd = gbranchmrg:format(repopath, repo.branch, branch)

        -- update list of local branches with remote one
        utils.exec(gbranchprune:format(repopath))
        if not utils.exec(cmd) then
            retcode = false
        end
    end
    return retcode
end

--- Get repos having taks commits.
-- roachme: Refactor it.
-- @return table of repos
function git.branch_ahead(id)
    -- roachme:FIXME: it checks diff between task branch and defaul branch.
    -- it should check uncommited changes instead.
    local res = {}
    local branch = taskunit.get(env.getcurr(), id, "branch")

    for _, repo in pairs(repos) do
        local repopath = config.aux.code .. repo.name
        local cmd = gdiff_commits:format(repopath, repo.branch, branch)

        if not is_repo_clean(repo.name) then
            -- has uncommited changes
            table.insert(res, repo.name)
        elseif not utils.exec(cmd) then
            --print("exec", cmd)
            -- has uncommited changes
            -- has commits ahead
            table.insert(res, repo.name)
        end
    end
    return res
end

--- Check that task branch exists and its has no uncommited changes.
-- @return on success - true
-- @return on failure - false
function git.check(id)
    local branch = taskunit.get(env.getcurr(), id, "branch")

    for _, repo in pairs(repos) do
        local repopath = config.aux.code .. repo.name
        local reponame = repo.name

        -- First off, check that task branch exists.
        if not _branch_exists(repopath, branch) then
            io.stderr:write(("branch '%s' doesn't exist\n"):format(branch))
            return false
        end

        -- Second off, check that branch has no uncommited changes.
        if not is_repo_clean(reponame) then
            local errmsg = ("repo '%s' uncommited changes\n"):format(reponame)
            io.stderr:write(errmsg)
            return false
        end

        -- Third off, check that task branch can be rebased against default
    end
    return true
end

--- Create commit according to pattern for repos.
-- @param id task ID
-- @return on success - true
-- @return on success - false
function git.commit_create(id)
    local cmd
    local desc = taskunit.get(env.getcurr(), id, "desc")

    -- roach: check that repo's branch is task branch.
    -- if not tryna switch to task branch.

    for _, repo in pairs(repos) do
        if not is_repo_clean(repo.name) then
            --print("repo", repo.name)
            local repopath = config.aux.code .. repo.name

            local part = changed_part(repo.name)
            local msg = ("[%s] %s: %s"):format(id, part, desc)

            -- check commit length and all
            if not commit_check(msg) then
                print("commit message checker failed")
                return false
            end

            cmd = ("git -C %s add . 1>/dev/null"):format(repopath)
            utils.exec(cmd)

            -- create a commit
            cmd = ("git -C %s commit -m '%s' 1>/dev/null"):format(repopath, msg)
            utils.exec(cmd)
        end
    end
    return true
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

--- Check that task branch exists.
function git.branch_exists(id)
    local branch = taskunit.get(env.getcurr(), id, "branch")

    for _, repo in pairs(repos) do
        local repopath = config.aux.code .. repo.name
        if not _branch_exists(repopath, branch) then
            return false
        end
    end
    return true
end

-- Public functions: end --

return git
