--- Terminal task manager.
-- @module tman

_G.progname = "tman"
_G.version = "v0.1.15"

local setup = require("core.setup")
local builtins = require("core.builtin")
local help = require("secondary.help")

---Tman interface.
local function main()
    local cmd = arg[1] or "help"

    -- POSIX getopt() does not let permutations as GNU version.
    table.remove(arg, 1)

    -- Call a command.
    for _, builtin in pairs(builtins) do
        if cmd == builtin.name then
            if builtin.setup_level == 1 then
                setup.gentle()
            elseif builtin.setup_level == 2 then
                setup.full()
            end
            return builtin.command()
        end
    end

    -- Command not found. Show some help.
    return help.usage(cmd)
end

os.exit(main())
