--- Terminal task manager.
-- @module tman

_G.progname = "tman"
_G.version = "v0.1.15"

local core = require("core.core")
local builtins = require("core.builtin")
local help = require("aux.help")

---Tman interface.
local function main()
    local cmd = table.remove(arg, 1) or "help"

    -- Call a command.
    for _, builtin in pairs(builtins) do
        if cmd == builtin.name then
            core.setup(builtin.setup_level)
            return builtin.command()
        end
    end

    -- Command not found. Show some help.
    return help.usage(cmd)
end

os.exit(main())
