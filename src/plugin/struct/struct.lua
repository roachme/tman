--- Form task filesystem structure.
-- Create repo symlinks, helper dirs, etc.
-- @module struct

local core = require("core.core")
local utils = require("aux.utils")
local errmod = require("core.errmod")
local plugin = require("core.plugin")

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

local function create_links(base) end

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
    create_links(struct_taskbase .. "/" .. id)
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

function struct.exec()
    local cmd = table.remove(arg, 1) or "create"
    local envname = core.getcurr().env
    local taskid = core.getcurr().curr

    if not envname then
        core.die(1, errmod.EECUR, "env")
    elseif not taskid then
        core.die(1, errmod.EICUR, "id")
    end

    local config = plugin.get_uconfig(envname).struct or {}
    if
        not struct.init(
            plugin.taskdir .. "/" .. envname,
            config.dirs or {},
            config.files or {}
        )
    then
        print("err: could not init plugin struct")
        os.exit(1)
    end

    if cmd == "create" then
        struct.create(taskid)
    elseif cmd == "delete" then
        struct.delete(taskid)
    elseif cmd == "help" then
        print("plugin struct: show some help")
    else
        core.die(1, errmod.EPKNON, cmd)
    end
end

return struct
