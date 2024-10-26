local utils = require("src.aux.utils")

local taskdir = {}

local tpath

---Init module.
---@param path string
---@return boolean
function taskdir.init(path)
    tpath = path
    return true
end

---Add task directory.
---@param env string
---@param id string
---@return boolean
function taskdir.add(env, id)
    return utils.mkdir(tpath .. "/" .. env .. "/" .. id)
end

---Delete task directory.
---@param env string
---@param id string
---@return boolean
function taskdir.del(env, id)
    return utils.rm(tpath .. "/" .. env .. "/" .. id)
end

---Rename task directory.
---@param env string
---@param oldid string
---@param newid string
---@return boolean
function taskdir.ren(env, oldid, newid)
    local oldpath = tpath .. "/" .. env .. "/" .. oldid
    local newpath = tpath .. "/" .. env .. "/" .. newid
    return utils.rename(oldpath, newpath)
end

return taskdir
