local ids = require("aux.iddb")
local core = require("core.core")
local git = require("secondary.git")
local utils = require("aux.utils")
local units = require("aux.unitdb")
local config = require("secondary.config")
local common = require("core.common")

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
    local fids = config.core.refs.ids

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
        local item = ids.getidx_generic(i)
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
end

function setup.setup()
    -- make sure tman structure is created and ok
    if not core.check() then
        common.die(1, "tman not inited", "tman")
    end

    -- download all need repos.
    local path = config.aux.code
    for _, repo in pairs(config.user.repos) do
        git.repo_clone(repo.link, repo.name, path)
    end

    --[[
    setup.setup_ids()
    setup.setup_units()
    ]]
end

--[[
setup levels
0 - no setup. Command `init' need, to create tman structure.
1 - gentle setup. Jost make sure tman structure is created and ok.
2 - gentle setup and database's ok.
3 - all above and config files ok.
]]

function setup.gentle()
    local path = config.aux.code

    if not core.check() then
        return common.die(1, "tman not inited\n", "setup")
    end

    -- roachme: maybe gotta move it to setup.full()
    -- make sure all user repos are downloaded.
    for _, repo in pairs(config.user.repos) do
        if not utils.access(path .. "/" .. repo.name) then
            if not git.repo_clone(repo.link, repo.name, path) then
                local errfmt = "could not download repo '%s'\n"
                return common.die(1, errfmt, "setup", repo.name)
            end
        end
    end

    return 0
end

function setup.full()
    setup.gentle()

    common.die(1, "full setup failed\n", "setup")
end

return setup
