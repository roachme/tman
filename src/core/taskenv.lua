local utils = require("aux.utils")
local dirent = require("posix.dirent")

local taskenv = {}
local dirpath

function taskenv.init(edir)
    dirpath = edir
    return true
end

---Get list of environments.
---@return table
function taskenv.list()
    local list = {}

    for _, dir in pairs(dirent.dir(dirpath)) do
        if dir ~= "." and dir ~= ".." then
            table.insert(list, dir)
        end
    end
    return list
end

---Check if environment exists.
---@param name string
---@return boolean
function taskenv.ext(name)
    for _, env in pairs(taskenv.list()) do
        if env == name then
            return true
        end
    end
    return false
end

---Check if environment name has illegal values.
---@param name string
---@return boolean
function taskenv.chk(name)
    return name == string.match(name, "^[%w]+[-]*[%w]+$")
end

---Add new environment.
---@param name string
---@return boolean
function taskenv.add(name)
    if not taskenv.chk(name) then
        return false
    end
    return utils.mkdir(dirpath .. "/" .. name)
end

---Delete an environment.
---@param name string
---@return boolean
function taskenv.del(name)
    if not taskenv.ext(name) then
        return false
    end
    return utils.rm(dirpath .. "/" .. name)
end

return taskenv
