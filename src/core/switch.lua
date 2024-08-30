local switch = {}

local fcurr, fprev
local specs = {}

--[[

curr:
env: ENV
curr: ID
prev: ID
]]

---Load special values.
---@param fname string
---@return table
local function load(fname)
    local res = {}
    local f = io.open(fname)

    if not f then
        return res
    end

    for line in f:lines() do
        local key, val = string.match(line, "(%w*): (%w*)")
        res[key] = val
        if res[key] == "" then
            res[key] = nil
        end
    end
    f:close()
    return res
end

---Save special values.
---@param fname string
---@param res table
---@return boolean
local function save(fname, res)
    local f = io.open(fname, "w")

    if not f then
        return false
    elseif not next(res) then
        -- nothing to save
        return true
    end

    f:write(("env: %s\n"):format(res.env))
    f:write(("curr: %s\n"):format(res.curr or ""))
    f:write(("prev: %s\n"):format(res.prev or ""))
    return f:close() == true
end

---Init module.
---@return boolean
function switch.init(dirpath)
    fcurr = dirpath .. "/" .. "curr"
    fprev = dirpath .. "/" .. "prev"
    specs = { curr = load(fcurr), prev = load(fprev) }
    return true
end

---Get current environment.
---@return table
function switch.env_getcurr()
    return {
        env = specs.curr.env,
        curr = specs.curr.curr,
        prev = specs.curr.prev,
    }
end

---Get previous environment.
---@return table
function switch.env_getprev()
    return {
        env = specs.prev.env,
        curr = specs.prev.curr,
        prev = specs.prev.prev,
    }
end

---Add new current environment.
---@param env table
---@return boolean
function switch.env_addcurr(env)
    local cenv = switch.env_getcurr()

    if env == specs.curr.env then
        print("!!!!! error: env_addcurr: the same envs")
        return false
    end

    specs.curr = { env = env.env, curr = env.curr, prev = env.prev }
    specs.prev = { env = cenv.env, curr = cenv.curr, prev = cenv.prev }
    return save(fcurr, specs.curr) and save(fprev, specs.prev)
end

---Swap current and previous environments.
---@return boolean
function switch.env_swapspec()
    local prevenv = switch.env_getprev()

    if not next(prevenv) then
        return false
    end
    switch.env_addcurr(prevenv)
    return true
end

---Delete current environment.
---@return boolean
function switch.env_delcurr()
    local curr = switch.env_getcurr()
    local prev = switch.env_getprev()

    if not curr then
        return false
    elseif not curr and not prev then
        return true
    end

    -- update special task environments.
    if curr and prev then
        local tmp = specs.curr
        specs.curr = prev
        specs.prev = tmp
    elseif curr and not prev then
        specs.curr = {}
    end

    --return save(fcurr, specs.curr)
    return save(fcurr, specs.curr) and save(fprev, specs.prev)
end

---Get current environment's current task id.
---@return string|nil
function switch.id_getcurr()
    return specs.curr.curr
end

---Get current environment's previous task id.
---@return string|nil
function switch.id_getprev()
    return specs.curr.prev
end

---Add new current task id.
---@param id string
---@return boolean
function switch.id_addcurr(id)
    local curr = switch.id_getcurr()

    -- prevent from duplicates: do nothing
    if curr == id then
        return true
    elseif not switch.env_getcurr().env then
        return false
    end

    specs.curr.curr = id
    specs.curr.prev = curr
    return save(fcurr, specs.curr) and save(fprev, specs.prev)
end

---Swap current and previous task ids.
---@return boolean
function switch.id_swapspec()
    local prev = switch.id_getprev()

    if not prev then
        return false
    end
    return switch.id_addcurr(prev)
end

---Delete current task id.
---@return boolean
function switch.id_delcurr()
    local env = switch.env_getcurr().env
    local curr = switch.id_getcurr()
    local prev = switch.id_getprev()

    if not env then
        return false
    elseif not curr and not prev then
        return true
    end

    -- update special task ids.
    if curr and prev then
        specs.curr.curr = prev
        specs.curr.prev = nil
    elseif curr and not prev then
        specs.curr.curr = nil
    end

    return save(fcurr, specs.curr)
end

function switch.id_delprev()
    local env = switch.env_getcurr().env
    local curr = switch.id_getcurr()
    local prev = switch.id_getprev()

    if not env then
        return false
    elseif not curr and not prev then
        return true
    end

    if not curr then
        return false
    elseif not curr or not prev then
        return false
    end

    specs.curr.prev = nil
    return save(fcurr, specs.curr)
end

return switch
