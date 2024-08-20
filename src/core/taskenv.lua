local envdb = require("aux.envdb")

local taskenv = {}
local fenv

local status = {
    CURR = 0,
    PREV = 1,
    ACTV = 2,
}

---Init env.
---@param fname string
function taskenv.init(fname)
    fenv = fname
    return envdb.init(fenv)
end

---Check that env exists in database.
---@param name string
---@return boolean
function taskenv.ext(name)
    return envdb.ext(name)
end

---Add an env.
---@param name string
---@param desc string
---@return boolean
function taskenv.add(name, desc)
    if envdb.ext(name) then
        return false
    end
    return envdb.add(name, desc, status.CURR)
end

---Delete an env.
---@param name string
---@return boolean
function taskenv.del(name)
    if not envdb.ext(name) then
        return false
    end
    return envdb.del(name)
end

---Get list of environments.
---@return table
function taskenv.list()
    return envdb.list()
end

---Check that environment name is valid.
---@param envname string
---@return boolean
function taskenv.chk(envname)
    --- roachme: gotta add an implementation
    if not envname then
        return false
    end
    return true
end

---Set environment value.
---@param key string
---@param val string
---@return boolean
function taskenv.set(key, val)
    return true
end

---Get environment unit values.
---@param name string
---@return table
function taskenv.get(name)
    return envdb.get(name)
end

return taskenv
