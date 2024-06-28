local ids = require("aux.iddb")
local git = require("core.git")
local utils = require("aux.utils")
local units = require("aux.unitdb")
local config = require("core.config")
local common = require("core.common")
local core = require("core.core")

local setup = {}

--[[
Found the logic in git project. File name setup.c:
    1. Run this module before any command. Pro'ly all but might use a flag to
       specify it.
    2. Make sure tman directory is safe to perform any command. It frees the
       rest of code logic from checks and crap like that.
]]

--[[

    1. Check that every task ID has corresponding. Yeah, all. It shouldn't take
       that much time is it seems. Tho the rest of the code'll run no error.
       If it slows down performance, rewrite it in C. Sounds good?
        a) unit file
        b) task dir
]]

--[[
local function ids_check()
    return true
end

local function ids_repair() end
]]

function setup.setup_ids()
    local fids = config.core.ids

    if not ids.init(fids) then
        common.die(1, "couldn't find file ids\n", "setup")
    end

    if not ids.check() then
        common.die(1, "file ids corrupted\n", "setup")
    end
end

function setup.setup_units()
    -- check that file exists and structure's ok
    -- also IIs match unit files
    local dirunit = config.core.units

    if not utils.access(dirunit) then
        common.die(1, "couldn't find directory units\n", "setup")
    end

    -- check that each task IDs has a file
    -- and its structure's ok
    for i = 1, ids.size() do
        local item = ids.getidx(i)
        local fname = dirunit .. item.id

        if not units.init(fname) then
            common.die(
                1,
                "couldn't find unit file for task id '%s'\n",
                "setup",
                item.id
            )
        elseif not units.check() then
            common.die(1, "%s: unit file ids corrupted\n", "setup", item.id)
        end
    end
end

function setup.setup_env()
    -- check that file exists and structure's ok
end

function setup.setup_config()
    -- check that file exists and structure's ok
    -- on: system and user
    config.check()
end

function setup.setup()
    setup.setup_ids()
    setup.setup_units()
    setup.setup_config()
end

--- Setup system wide thingies.
function setup.sys()
    if not core.check() then
        if not core.init() then
            io.stderr:write("tman: fatal: couldn't inited\n")
            os.exit(1)
        end
    end

    setup.setup_ids()
    setup.setup_units()
    setup.setup_config()
end

--- Setup user wide thingies like stuff from user config.
function setup.user()
    -- make sure all repos in user config are downloaded
    -- roachme: maybe git.repo_clone() shouldn't check that repo exists?..
    git.repo_clone()
end

return setup
