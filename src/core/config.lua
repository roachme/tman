--- Parse config file and provide env for the rest of the code.
-- @module config

local env = require("core.env")
local utils = require("aux.utils")
local sysconfig = require("aux.sysconfig")
local userconfig = require("aux.userconfig")

local config = {}

local function find_config_file(fname)
    local userhome = os.getenv("HOME")
    local configpath = userhome .. "/.config/tman/" .. fname

    if utils.access(configpath) then
        return configpath
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

function config.load()
    local prefix
    local envname
    sysconfig.init(fsysconf)

    -- get system config values
    prefix = sysconfig.get("prefix")

    env.init(prefix .. "/.tman/env.list")
    envname = env.getcurr()

    -- load stuff from diff modules
    config.sys = sysconfig.getvars()
    config.user = userconfig.getvars()

    -- roachme: maybe it's better to move it to struct.lua
    config.core = {
        name = ".tman",
        ids = prefix .. "/.tman/" .. envname .. "/ids", -- it's a file
        units = prefix .. "/.tman/" .. envname .. "/units/",
        path = prefix .. "/.tman/" .. envname,
        prefix = prefix,
    }

    config.aux = {
        code = prefix .. "/" .. envname .. "/code/",
        tasks = prefix .. "/" .. envname .. "/tasks/",
    }

    -- roachme: hotfixes
    config.sys.fenv = prefix .. "/.tman/env.list" -- list of envs
    config.sys.env = envname
end

config.load()

return config
