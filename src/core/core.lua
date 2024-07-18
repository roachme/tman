--- Tman core module to to init, check and repair itself.
-- @module core

local utils = require("aux.utils")
local config = require("aux.config")

local core = {}

local prefix = config.prefix .. "/"

core.struct = {
    prefix = { isdir = false, iscore = true, path = prefix },
    dbdir = { isdir = false, iscore = true, path = prefix .. ".tman/" },
    curr = { isdir = false, iscore = true, path = prefix .. ".tman/curr" },

    units = { isdir = true, iscore = true, path = prefix .. ".tman/units/" },

    refs = { isdir = true, iscore = true, path = prefix .. ".tman/refs/" },
    envs = { isdir = false, iscore = true, path = prefix .. ".tman/refs/envs" },
    ids = { isdir = false, iscore = true, path = prefix .. ".tman/refs/ids" },

    plugin = { isdir = true, iscore = false, path = prefix .. ".tman/plugin/" },

    tasks = { isdir = true, iscore = false, path = prefix .. "tasks/" },
}

function core.die(exit_code, errfmt, ...)
    local errmsg = ("%s: %s: " .. errfmt .. "\n"):format(progname, ...)
    io.stderr:write(errmsg)
    os.exit(exit_code)
end

---Check tman core and base directories ain't corrupted and exist.
---@return boolean
function core.check()
    for _, file in pairs(core.struct) do
        local path = file.path
        if not utils.access(path) then
            return false
        end
    end
    return true
end

---Create tman core and base directories.
---@return boolean
function core.create()
    for _, file in pairs(core.struct) do
        local path = file.path
        if file.isdir then
            if not utils.mkdir(path) then
                return false
            end
        end
    end

    for _, file in pairs(core.struct) do
        local path = file.path
        if not file.isdir then
            if not utils.touch(path) then
                return false
            end
        end
    end
    return true
end

--- Backup data.
-- @param fname archive filename (default extention is .tar)
-- @param repo_included whether or not include repos in archive
-- @return on success - true
-- @return on failure - false
function core.backup(fname, repo_included)
    -- roachme: run git gc before including repos in archive so it takes less place.
    -- roachme: Replace codebase with value from config
    local cmd

    if repo_included then
        cmd = ("tar -C %s -czf %s.tar ."):format(config.base, fname)
    else
        cmd = ("tar -C %s --exclude=codebase -czf %s.tar ."):format(
            config.base,
            fname
        )
    end
    return utils.exec(cmd)
end

--- Restore archive.
-- @param fname archive fname
-- @return on success - true
-- @return on failure - false
function core.restore(fname)
    local cmd = ("tar -xf %s -C %s"):format(fname, config.base)

    if not utils.access(fname) then
        io.stderr:write(("'%s': no such tar file\n"):format(fname))
        return false
    end

    print("delete current structure")
    utils.rm(config.taskbase)
    utils.rm(config.codebase)
    utils.rm(config.tmanbase)

    print("copy backup structure")
    if not utils.exec(cmd) then
        io.stderr:write("failed to execute archive command\n")
        return false
    end
    return true
end

--[[
Found the logic in git project. File name setup.c:
    1. Run this module before any command. Pro'ly all but might use a flag to
       specify it.
    2. Make sure tman directory is safe to perform any command. It frees the
       rest of code logic from checks and crap like that.
]]

--[[

    1. Check that every task id has corresponding. Yeah, all. It shouldn't take
       that much time is it seems. Tho the rest of the code'll run no error.
       If it slows down performance, rewrite it in C. Sounds good?
        a) unit file
        b) task dir
]]

--[[
setup levels
0 - no setup. Command `init' need, to create tman structure.
1 - gentle setup. Jost make sure tman structure is created and ok.
2 - gentle setup and database's ok.
3 - all above and config files ok.
]]
---Basic system check (level: 1).
local function setup_basic()
    if not core.check() then
        return core.die(1, "tman not inited", "setup")
    end
    return 0
end

---Strict system check (level: 2).
local function setup_strict()
    setup_basic()

    -- TODO: make sure database ain't corrupted.
end

---Full system and user lands check (level: 3).
local function setup_full()
    setup_strict()

    -- TODO: make sure (user) config stuff are valid
    -- user custom stuff make sense like brachpatt, etc.
end

---Setup database stuff before execute a command.
---@param level number
function core.setup(level)
    if level == 0 then
        -- do nothing
        return 0
    elseif level == 1 then
        return setup_basic()
    elseif level == 2 then
        return setup_strict()
    elseif level == 3 then
        return setup_full()
    end
    return core.die(1, "no such setup level '%d'", "setup", level)
end

return core
