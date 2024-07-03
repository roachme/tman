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
local fusreconf = find_config_file("tman_conf.lua")

function config.check()
    if not fsysconf then
        io.stderr:write("tman: sys.conf: system tmanconf missing\n")
        os.exit(1)
    end
    if not fusreconf then
        io.stderr:write("tman: user.conf: user config missing\n")
        os.exit(1)
    end
end

function config.show()
    print("base", config.base)
    print("install", config.install)
    print("brpanchpatt", config.branchpatt)
    io.write("struct dirs: { ")
    for _, dir in pairs(config.struct.dirs) do
        io.write(dir, " ")
    end
    print("}")
    io.write("struct files: { ")
    for _, file in pairs(config.struct.files) do
        io.write(file, " ")
    end
    print("}")

    print("repos: {")
    for _, item in pairs(config.repos) do
        print("  {", item.name, item.branch, item.path or "", "}")
    end
    print("}")
end

function config.load()
    local prefix, env
    sysconfig.init(fsysconf)

    -- get system config values
    prefix = sysconfig.get("prefix")
    env = sysconfig.get("env")

    -- load stuff from diff modules
    config.sys = sysconfig.getvars()
    config.user = userconfig.getvars()

    -- roachme: maybe it's better to move it to struct.lua
    config.core = {
        name = ".tman",
        ids = prefix .. "/.tman/ids",
        units = prefix .. "/.tman/units/",
        path = prefix .. "/.tman/",
    }

    config.aux = {
        code = prefix .. "/code/",
        tasks = prefix .. "/tasks/",
    }

    -- roachme: hotfixes
    config.sys.fenv = config.core.path .. "envs"
    config.sys.env = env -- current env name
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
