--- Tman core module to to init, check and repair itself.
-- @module core

local utils = require("aux.utils")
local config = require("secondary.config")
local git = require("secondary.git")
local env = require("core.env")

local core = {}

function core.struct()
    -- tman core structure
    utils.mkdir(config.core.path)
    utils.mkdir(config.core.refs.base)
    utils.mkdir(config.core.units)

    utils.touch(config.core.curr)
    utils.touch(config.core.refs.ids)
    utils.touch(config.core.refs.envs)

    -- tman aux structure
    utils.mkdir(config.aux.code)
    utils.mkdir(config.aux.tasks)
end

function core.env()
    local defenv = "tman"

    env.init(config.core.refs.envs)
    if not env.exists(defenv) then
        if not env.add(defenv, "main tman env") then
            io.stderr:write("couldn't add default env\n")
            os.exit(1)
        end
    end
end

--- Check tman dir ain't corrupted and exists.
-- @return true on success, otherwise false
function core.check()
    -- roachme:
    -- return 1: tman core stuff are corrupted
    -- retrun 2: tman base stuff are corrupted
    -- NOT dirs / files
    local files = {
        config.ids,
        --config.tmanconf,
    }
    local dirs = {
        config.core.units,
        config.core.path,
        config.aux.code,
        config.aux.tasks,
    }

    for _, dir in pairs(dirs) do
        if not utils.access(dir) then
            return 1
        end
    end
    for _, file in pairs(files) do
        if not utils.access(file) then
            return 1
        end
    end

    local dir = config.aux.code
    for _, repo in pairs(config.user.repos) do
        if not utils.access(dir .. repo.name) then
            return 1
        end
    end
    return 0
end

---Init system to use a util.
function core.init()
    if core.check() == 0 then
        return 0
    end
    -- create tman structure
    core.struct()

    -- add default env
    core.env()

    -- download repos
    local dir = config.aux.code
    local errfmt = "tman: %s: couldn't download repo\n"
    for _, repo in pairs(config.user.repos) do
        if not utils.access(dir .. repo.name) then
            if not git.repo_clone(repo.link, repo.name, dir) then
                io.stderr:write(errfmt:format(repo.name))
                os.exit(1)
            end
            print(("%s: repo downloaded"):format(repo.name))
        end
    end
    return 0
end

function core.repair() end

--- Backup data.
-- @param fname archive filename (default extention is .tar)
-- @param repo_included whether or not include repos in archive
-- @return on success - true
-- @return on failure - false
function core.backup(fname, repo_included)
    -- roachme: run git gc before including repos in archive so it takes less place.
    -- roachme: Replace codebase with value from config
    local cmd

    if repo_included then
        cmd = ("tar -C %s -czf %s.tar ."):format(config.base, fname)
    else
        cmd = ("tar -C %s --exclude=codebase -czf %s.tar ."):format(
            config.base,
            fname
        )
    end
    return utils.exec(cmd)
end

--- Restore archive.
-- @param fname archive fname
-- @return on success - true
-- @return on failure - false
function core.restore(fname)
    local cmd = ("tar -xf %s -C %s"):format(fname, config.base)

    if not utils.access(fname) then
        io.stderr:write(("'%s': no such tar file\n"):format(fname))
        return false
    end

    print("delete current structure")
    utils.rm(config.taskbase)
    utils.rm(config.codebase)
    utils.rm(config.tmanbase)

    print("copy backup structure")
    if not utils.exec(cmd) then
        io.stderr:write("failed to execute archive command\n")
        return false
    end
    return true
end

return core
