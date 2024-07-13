--- Parse config file and provide env for the rest of the code.
-- @module config

local utils = require("aux.utils")
local sysconfig = require("aux.sysconfig")
local userconfig = require("aux.userconfig")

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

local fsysconf = find_config_file("sys.conf")

function config.check()
    if not fsysconf then
        io.stderr:write("tman: sys.conf: system tmanconf missing\n")
        os.exit(1)
    end
end

function config.load()
    sysconfig.init(fsysconf)
    --userconfig.init(fusreconf)

    -- load stuff from diff modules
    config.sys = sysconfig.getvars()
    config.user = userconfig
    config.prefix = sysconfig.get("prefix")
end

---@param key string
---@return string|table
function config.getsys(key)
    return config.sys[key]
end

---@param key string
---@param val string
function config.setsys(key, val)
    sysconfig.set(key, val)
    config.load()
end

config.check() -- roachme: should be done in core.lua or setup.lua
config.load()

return config
