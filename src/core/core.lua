--- Tman core module to to init, check and repair itself.
-- @module core

local utils = require("aux.utils")
local config = require("secondary.config")
local env = require("core.env")

local core = {}

local tman_struct = {
    { isdir = false, core = true, name = ".tman/curr" },
    { isdir = true, core = true, name = ".tman/units" },
    { isdir = true, core = true, name = ".tman/refs" },
    { isdir = false, core = true, name = ".tman/refs/envs" },
    { isdir = false, core = true, name = ".tman/refs/envs" },
    { isdir = true, core = false, name = "code" },
    { isdir = true, core = false, name = "tasks" },
}

---Check tman core and base directories ain't corrupted and exist.
---@return boolean
function core.check()
    for _, file in pairs(tman_struct) do
        local path = config.prefix .. "/" .. file.name
        if not utils.access(path) then
            return false
        end
    end
    return true
end

---Create tman core and base directories.
---@return boolean
function core.create()
    -- create base and aux directories.
    if not utils.mkdir(config.prefix) then
        return false
    end
    if not utils.mkdir(config.prefix .. "/.tman") then
        return false
    end

    for _, file in pairs(tman_struct) do
        local path = config.prefix .. "/" .. file.name
        if file.isdir then
            if not utils.mkdir(path) then
                return false
            end
        else
            if not utils.touch(path) then
                return false
            end
        end
    end
    return true
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
