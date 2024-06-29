--- Parse config file and provide env for the rest of the code.
-- @module config

local shell = require("aux.shell")
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

    -- get system config values
    sysconfig.init(fsysconf)
    prefix = sysconfig.get("prefix")

    -- get env name

    shell.init(prefix .. "/.tman")
    envname = shell.getenv()

    -- load stuff from diff modules
    config.sys = sysconfig.getvars()
    config.user = userconfig.getvars()

    -- roachme: maybe it's better to move it to struct.lua
    config.core = {
        prefix = prefix .. "/",
        ids = prefix .. "/.tman/" .. envname .. "/ids", -- it's a file
        units = prefix .. "/.tman/" .. envname .. "/units/",

        -- shell stuff
        env = prefix .. "/.tman/env",
        envs = prefix .. "/.tman/envs",
        curr = prefix .. "/.tman/curr",
        base = prefix .. "/.tman/",
    }

    config.aux = {
        code = config.core.prefix .. envname .. "/code/",
        tasks = config.core.prefix .. envname .. "/tasks/",
    }
end

config.load()

return config
