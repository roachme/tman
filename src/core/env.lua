--- Module to edit and manipulate environment.
-- Create default env, if none's presented.
-- @module env

local envdb = require("aux.envdb")
local shell = require("aux.shell")
local config = require("core.config")

local env = {}

-- Hold current and previous env names to minimize lookup.
local curr, prev

local status = {
    CURR = 0,
    PREV = 1,
    ACTV = 2,
}

local function unset(name)
    envdb.set(name, status.ACTV)
end

local function load_spec_envs()
    for i = 1, envdb.size() do
        local item = envdb.getix(i)
        if item.status == status.CURR then
            curr = item.name
        elseif item.status == status.PREV then
            prev = item.name
        end
    end
end

function env.exists(name)
    return envdb.exists(name)
end

function env.swap()
    local tmpprev = prev

    prev = curr
    curr = tmpprev

    -- roachme:BUG: gotta unset old prev.
    unset(prev)
    unset(curr)

    -- roachme: it save into file.
    -- Gotta change that logic to minimize writing into file.
    envdb.set(prev, status.PREV)
    envdb.set(curr, status.CURR)

    -- update sys.conf
    return true
end

function env.setcurr(name)
    if not envdb.exists(name) then
        return false
    end

    prev = curr
    curr = name

    envdb.set(prev, status.PREV)
    envdb.set(curr, status.CURR)

    -- update file for shell part of util.
    shell.setenv(name)
    return true
end

function env.add(name, desc)
    if envdb.exists(name) then
        return false
    end

    envdb.add(name, desc)
    return env.setcurr(name)
end

function env.get(name)
    for i = 1, envdb.size() do
        local item = envdb.getix(i)
        if item.name == name then
            return item
        end
    end
    return {}
end

function env.getcurr()
    return curr
end

function env.getprev()
    return prev
end

function env.set(name, stat)
    for i = 1, envdb.size() do
        local item = envdb.getix(i)
        if item.name == name then
            envdb.set(name, stat)
            return true
        end
    end
    return false
end

function env.del(name)
    if not envdb.exists(name) then
        return false
    end

    envdb.del(name)
    if name == curr then
        env.swap()
    end
    return true
end

function env.list()
    if curr then
        local item = envdb.get(curr)
        print(("* %-10s %s"):format(item.name, item.desc))
    end
    if prev then
        local item = envdb.get(prev)
        print(("- %-10s %s"):format(item.name, item.desc))
    end

    for i = 1, envdb.size() do
        local item = envdb.getix(i)
        -- skip special env names (prev & curr)
        if item.name ~= curr and item.name ~= prev then
            print(("  %-10s %s"):format(item.name, item.desc))
        end
    end
    return true
end

envdb.init(config.core.envs)
shell.init(config.core.base)
load_spec_envs()

return env
