--- Form task filesystem structure.
-- Create repo symlinks, helper dirs, etc.
-- @module struct

local utils = require("aux.utils")
local config = require("secondary.config")
local core = require("core.core")

local struct = {}

local uconfig

-- Private functions: end --

--- Create dirs.
-- @param base directry structure base
local function create_dirs(base)
    for _, dir in pairs(uconfig.struct.dirs) do
        utils.mkdir(base .. "/" .. dir)
    end
end

--- Create files.
-- @param base file structure base
local function create_files(base)
    for _, file in pairs(uconfig.struct.files) do
        utils.touch(base .. file)
    end
end

--- Create symlinks to repos.
-- @param id task id
local function link_repos(repobase, taskbase)
    for _, repo in pairs(uconfig.repos) do
        local target = repobase .. repo.name
        local linkname = taskbase .. repo.name
        utils.link(target, linkname)
    end
end

-- Private functions: end --

-- Public functions: start --

--- Create task filesystem structure.
-- @param id task id
-- @return on success - true
-- @return on failure - false
function struct.create(envname, id)
    uconfig = config.uget(envname)
    local dirname = utils.genname(envname, id)
    local taskdir = core.struct.tasks.path .. dirname
    local notedir = core.struct.tasks.path .. dirname .. "/notes/"
    local repodir = core.struct.tasks.path .. dirname .. "/repos/"

    utils.mkdir(dirname)
    utils.mkdir(notedir)
    utils.mkdir(taskdir)
    utils.mkdir(repodir)

    create_dirs(taskdir)
    create_files(notedir)
    link_repos(core.struct.code.path, repodir)
    return true
end

--- Delete task filesystem structure.
-- @param id task id
-- @return on success - true
-- @return on failure - false
function struct.delete(envname, id)
    local dirname = utils.genname(envname, id)
    local taskdir = core.struct.tasks.path .. "/" .. dirname
    return utils.rm(taskdir)
end

--- Rename task dir.
-- @param oldid old task id
-- @param newid new task id
-- @return on success - true
-- @return on failure - false
function struct.rename(envname, oldid, newid)
    local old_dir = core.struct.units.path .. utils.genname(envname, oldid)
    local new_dir = core.struct.units.path .. utils.genname(envname, newid)
    return utils.rename(old_dir, new_dir) == 0
end

-- Public functions: end --

return struct
