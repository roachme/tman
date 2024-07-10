--- Form task filesystem structure.
-- Create repo symlinks, helper dirs, etc.
-- @module struct

local utils = require("aux.utils")
local config = require("secondary.config")

local struct = {}

-- Private functions: end --

--- Create dirs.
-- @param base directry structure base
local function create_dirs(base)
    for _, dir in pairs(config.user.struct.dirs) do
        utils.mkdir(base .. "/" .. dir)
    end
end

--- Create files.
-- @param base file structure base
local function create_files(base)
    for _, file in pairs(config.user.struct.files) do
        utils.touch(base .. file)
    end
end

--- Create symlinks to repos.
-- @param id task ID
local function link_repos(id)
    for _, repo in pairs(config.user.repos) do
        local reponame = repo.name
        local target = config.aux.code .. reponame
        local linkname = config.aux.tasks .. id .. "/repos/" .. reponame

        -- check if there's invalid link, if so then delete it.
        if not utils.access(linkname) then
            -- use module os cuz it might delete repo itself.
            os.remove(linkname)
        end
        utils.link(target, linkname)
    end
end

-- Private functions: end --

-- Public functions: start --

--- Create task filesystem structure.
-- @param id task ID
-- @return on success - true
-- @return on failure - false
function struct.create(envname, id)
    local dirname = utils.genname(envname, id)
    local taskdir = config.aux.tasks .. dirname
    local notedir = config.aux.tasks .. dirname .. "/notes/"
    local repodir = config.aux.tasks .. dirname .. "/repos/"

    utils.mkdir(dirname)
    utils.mkdir(notedir)
    utils.mkdir(taskdir)
    utils.mkdir(repodir)

    create_dirs(taskdir)
    create_files(notedir)
    link_repos(id)
    return true
end

--- Delete task filesystem structure.
-- @param id task ID
-- @return on success - true
-- @return on failure - false
function struct.delete(envname, id)
    local dirname = utils.genname(envname, id)
    local taskdir = config.aux.tasks .. "/" .. dirname
    return utils.rm(taskdir)
end

--- Rename task dir.
-- @param oldid old task ID
-- @param newid new task ID
-- @return on success - true
-- @return on failure - false
function struct.rename(envname, oldid, newid)
    local old_dir = config.core.units .. utils.genname(envname, oldid)
    local new_dir = config.core.units .. utils.genname(envname, newid)
    return utils.rename(old_dir, new_dir) == 0
end

-- Public functions: end --

return struct
