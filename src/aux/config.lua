--- Parse config file and provide env for the rest of the code.
-- @module config

local utils = require("aux.utils")
local sconfig = require("aux.sysconfig")
local uconfig = require("aux.userconfig")

local config = {}

local function find_config_file(fname)
    local userhome = os.getenv("HOME")
    local confpathes = {
        userhome .. "/" .. ".tman/" .. fname,
        userhome .. "/" .. ".config/tman/" .. fname,
    }
    for _, conf in pairs(confpathes) do
        if utils.access(conf) then
            return conf
        end
    end
    return nil
end

local fsysconf = find_config_file("sys2.conf")

function config.check()
    if not fsysconf then
        io.stderr:write("tman: sys.conf: system tmanconf missing\n")
        os.exit(1)
    end
end

function config.load()
    sconfig.init(fsysconf)
    --userconfig.init(fusreconf)

    -- load stuff from diff modules
    config.sys = sconfig.getvars()
    config.user = uconfig
    config.prefix = sconfig.get("prefix")
end

---@param key string
---@return string|table
function config.sget(key)
    return config.sys[key]
end

---@param key string
---@param val string
function config.sset(key, val)
    sconfig.set(key, val)
    config.load()
end

function config.uget(envname)
    return uconfig.get(envname)
end

function config.uset(envname, key, val)
    return uconfig.set(envname, key, val)
end

config.check() -- roachme: should be done in core.lua or setup.lua
config.load()

return config
