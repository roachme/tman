--- Form task filesystem structure.
-- Create repo symlinks, helper dirs, etc.
-- @module struct

local utils = require("aux.utils")

local struct = {}
local struct_taskbase, struct_dirs, struct_files

---Create dirs.
---@param base string
local function create_dirs(base)
    for _, dir in pairs(struct_dirs) do
        utils.mkdir(base .. "/" .. dir)
    end
end

---Create files.
---@param base string
local function create_files(base)
    for _, file in pairs(struct_files) do
        utils.touch(base .. "/" .. file)
    end
end

---Init plugin struct.
---@param taskbase string
---@param dirs table
---@param files table
---@return boolean
function struct.init(taskbase, dirs, files)
    struct_taskbase = taskbase
    struct_dirs = dirs
    struct_files = files
    return true
end

---Create task filesystem structure.
---@param id string
---@return boolean
function struct.create(id)
    create_dirs(struct_taskbase .. "/" .. id)
    create_files(struct_taskbase .. "/" .. id)
    return true
end

---Delete directories and files defined in config.
---@param id string
---@return boolean
function struct.delete(id)
    for _, dir in pairs(struct_dirs) do
        utils.rm(struct_taskbase .. "/" .. id .. "/" .. dir)
    end
    for _, file in pairs(struct_files) do
        utils.rm(struct_taskbase .. "/" .. id .. "/" .. file)
    end
    return true
end

---Rename task dir.
---@param oldid string
---@param newid string
---@return boolean
function struct.rename(oldid, newid)
    local old_dir = struct_taskbase .. "/" .. oldid
    local new_dir = struct_taskbase .. "/" .. newid
    return utils.rename(old_dir, new_dir) == 0
end

return struct
