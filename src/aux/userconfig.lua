--- Module to parse, check and load user config.
-- @module userconfig

local json = require("cjson")
local utils = require("aux.utils")

local default_repos = {}
local default_branchpatt = "TYPE/ID_DESC_DATE"
local default_commitpatt = "[ID] PART: MSG"
local default_struct = {
    dirs = {},
    files = {},
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
local userconf = json.decode(read_file(fconf))

userconf.repos = userconf.repos or default_repos

userconf.struct = userconf.struct or default_struct
userconf.struct.dirs = userconf.struct.dirs or {}
userconf.struct.files = userconf.struct.files or {}

userconf.branchpatt = userconf.branchpatt or default_branchpatt
userconf.commitpatt = userconf.commitpatt or default_commitpatt

-- TODO: do some checks
-- code goes here...
-- 1. check that struct has members: dirs and files
-- 2. check that repos has members: link, name, etc.

return userconf
