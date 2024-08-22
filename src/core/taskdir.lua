local utils = require("aux.utils")

local taskdir = {}

local tpath

function taskdir.init(path)
    tpath = path
    return true
end

function taskdir.add(envname, id)
    return utils.mkdir(tpath .. "/" .. envname .. "/" .. id)
end

function taskdir.del(envname, id)
    return utils.rm(tpath .. "/" .. envname .. "/" .. id)
end

return taskdir
