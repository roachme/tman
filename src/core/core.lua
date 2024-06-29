--- Tman core module to to init, check and repair itself.
-- @module core

local env = require("core.env")
local git = require("core.git")
local utils = require("aux.utils")
local config = require("core.config")

local core = {}

local dirs = {
    config.aux.code,
    config.aux.tasks,
    config.core.base,
    config.core.units,
}

local files = {
    config.core.ids,
    config.core.env,
    config.core.envs,
    config.core.curr,
}

function core.struct()
    utils.mkdir(config.core.base)

    for _, dir in pairs(dirs) do
        utils.mkdir(dir)
    end

    for _, file in pairs(files) do
        utils.touch(file)
    end
end

--- Check tman dir ain't corrupted and exists.
-- @return true on success, otherwise false
function core.check()
    for _, dir in pairs(dirs) do
        if not utils.access(dir) then
            return false
        end
    end
    for _, file in pairs(files) do
        if not utils.access(file) then
            return false
        end
    end
    return true
end

--- Init system to use a util.
function core.init()
    local def_envname = "work"

    if core.check() == 0 then
        return
    end

    -- create tman structure
    core.struct()

    -- add default env name
    env.add(def_envname, "main tman env")

    -- download repos if needed
    git.repo_clone()
    return 1
end

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
