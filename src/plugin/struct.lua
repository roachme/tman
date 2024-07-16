--- Form task filesystem structure.
-- Create repo symlinks, helper dirs, etc.
-- @module struct

local utils = require("aux.utils")
local config = require("struct.config")
local core = require("core.core")

local struct = {}
local uconfig

---Create dirs.
---@param base string
local function create_dirs(base)
    for _, dir in pairs(uconfig.struct.dirs) do
        utils.mkdir(base .. "/" .. dir)
    end
end

---Create files.
---@param base string
local function create_files(base)
    for _, file in pairs(uconfig.struct.files) do
        utils.touch(base .. "/" .. file)
    end
end

---Create task filesystem structure.
---@param envname string
---@param id string
---@return boolean
function struct.create(envname, id)
    uconfig = config.uget(envname)
    local dirname = utils.genname(envname, id)
    local taskdir = core.struct.tasks.path .. dirname

    utils.mkdir(taskdir)
    create_dirs(taskdir)
    create_files(taskdir)
    return true
end

---Delete task filesystem structure.
---@param envname string
---@param id string
---@return boolean
function struct.delete(envname, id)
    local dirname = utils.genname(envname, id)
    local taskdir = core.struct.tasks.path .. "/" .. dirname
    return utils.rm(taskdir)
end

---Rename task dir.
---@param envname string
---@param oldid string
---@param newid string
---@return boolean
function struct.rename(envname, oldid, newid)
    local old_dir = core.struct.units.path .. utils.genname(envname, oldid)
    local new_dir = core.struct.units.path .. utils.genname(envname, newid)
    return utils.rename(old_dir, new_dir) == 0
end

return struct
