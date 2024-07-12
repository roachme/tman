--local utils = require("aux.utils")

local commit = {}

--[[
local items = {
    "ID",
    "PART",
    "MSG",
}

local function commit_check(msg)
end

---Generate commit message.
local function genmsg(msgpatt)
    msgpatt = msgpatt or "[ID] PART: MSG"
end

---Create commit.
---@param reponame string
---@param msgpatt string
---@param msg string
---@param repopath string | nil
---@return boolean
function commit.create2(reponame, msgpatt, msg, repopath)
    return true
end

---Create commit.
---@param reponame string
---@param path string | nil
---@return boolean
function commit.create(reponame, path)
    path = path or "."
    local fmt = "git -C %s/%s git add ."
    local cmd = string.format(fmt, path, reponame)

    -- add all changes. maybe it's better a user to choose what he wants.
    utils.exec(cmd)


    return true
end

--- Get changed part of the code.
local function changed_files(reponame)
    local files = {}
    local repopath = core.struct.code.path .. reponame
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

--commit message pattern
-- [<ticket ID>] <изменяемая часть>: <краткое описание>

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
--- Create commit according to pattern for repos.
-- @param id task id
-- @return on success - true
-- @return on success - false
function git.commit_create(id)
    local cmd
    local desc = taskunit.get(env.getcurr(), id, "desc")

    -- roach: check that repo's branch is task branch.
    -- if not tryna switch to task branch.

    for _, repo in pairs(repos) do
        --print("repo", repo.name)
        local repopath = core.struct.code.path .. repo.name

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
    return true
end
]]

return commit
