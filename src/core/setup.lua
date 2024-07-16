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
function setup.basic()
    if not core.check() then
        return core.die(1, "tman not inited", "setup")
    end

    return 0
end

---Strict system check (level: 2).
function setup.strict()
    setup.basic()

    -- TODO: make sure database ain't corrupted.
end

---Full system and user lands check (level: 3).
function setup.full()
    setup.strict()

    -- TODO: make sure (user) config stuff are valid
    -- user custom stuff make sense like brachpatt, etc.
end

return setup
