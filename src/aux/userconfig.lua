--- Module to parse, check and load user config.
-- @module userconfig

local json = require("cjson")
local utils = require("aux.utils")

local uconfig = {}

local default_repos = {}
local default_branchpatt = "TYPE/ID_DESC_DATE"
local default_commitpatt = "[ID] PART: MSG"
local default_struct = {
    dirs = {},
    files = {},
}
local default_env = {
    repos = default_repos,
    struct = {
        dirs = {},
        files = {},
    },
    branchpatt = default_branchpatt,
    commitpatt = default_commitpatt,
}

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

local function read_file(fname)
    local res
    local f = io.open(fname)

    if not f then
        return "{}"
    end

    res = f:read("*a")
    f:close()
    return res ~= "" and res or "{}"
end

local fconf = find_config_file("user.json")
local config = json.decode(read_file(fconf))

local function config_load()
    for envname, _ in pairs(config) do
        config[envname].repos = config[envname].repos or default_repos

        config[envname].struct = config[envname].struct or default_struct
        config[envname].struct.dirs = config[envname].struct.dirs or {}
        config[envname].struct.files = config[envname].struct.files or {}

        config[envname].branchpatt = config[envname].branchpatt
            or default_branchpatt
        config[envname].commitpatt = config[envname].commitpatt
            or default_commitpatt
    end
end

function uconfig.get(envname)
    if not envname or not config[envname] then
        return default_env
    end
    return config[envname]
end

-- TODO: do some checks
-- code goes here...
-- 1. check that struct has members: dirs and files
-- 2. check that repos has members: link, name, etc.

config_load()

return uconfig
