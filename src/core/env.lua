local envdb = require("aux.envdb")

local env = {}
local fenv

local status = {
    CURR = 0,
    PREV = 1,
    ACTV = 2,
}

---Init env.
---@param fname string
function env.init(fname)
    fenv = fname
    envdb.init(fenv)
end

---Get previous env.
---@return string | nil
function env.getprev()
    for i = 1, envdb.size() do
        local item = envdb.getidx(i)
        if item.status == status.PREV then
            return item.name
        end
    end
    return nil
end

---Get current env.
---@return string | nil
function env.getcurr()
    for i = 1, envdb.size() do
        local item = envdb.getidx(i)
        if item.status == status.CURR then
            return item.name
        end
    end
    return nil
end

---Mark env as current.
---@param name string
---@return boolean
function env.setcurr(name)
    local curr = env.getcurr()
    local prev = env.getprev()

    if not envdb.exists(name) then
        return false
    end
    -- sentinel guard: don't mark the same id as current
    if curr == name then
        return true
    end

    -- if theree's previous task then unmark it
    if prev then
        envdb.set(prev, status.ACTV)
    end

    if curr then
        envdb.set(curr, status.PREV)
    end
    envdb.set(name, status.CURR)
    return true
end

---Swap current and previous env names.
---Only if both current and previous exist.
---@return boolean
function env.swap()
    local newcurr = env.getprev()

    if newcurr then
        return env.setcurr(newcurr)
    end
    return false
end

---Check that env exists in database.
---@param name string
---@return boolean
function env.exists(name)
    return envdb.exists(name)
end

---Add an env.
---@param name string
---@param desc string
---@return boolean
function env.add(name, desc)
    if not envdb.add(name, desc, status.CURR) then
        return false
    end
    return env.setcurr(name)
end

---Delete an env.
---@param name string
---@return boolean
function env.del(name)
    local curr = env.getcurr()
    local prev = env.getprev()

    if not envdb.exists(name) then
        return false
    end

    envdb.del(name)

    if name == curr and prev then
        envdb.set(prev, status.CURR)
    end
    return true
end

---List envs.
function env.list()
    local prev = env.getprev()
    local curr = env.getcurr()

    if curr then
        local item = envdb.get(curr)
        print(("* %-10s %s"):format(item.name, item.desc))
    end
    if prev then
        local item = envdb.get(prev)
        print(("- %-10s %s"):format(item.name, item.desc))
    end

    for i = 1, envdb.size() do
        local item = envdb.getidx(i)

        if item.name ~= curr and item.name ~= prev then
            print(("a %-10s %s"):format(item.name, item.desc))
        end
    end
end

return env
