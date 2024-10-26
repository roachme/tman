---Task manager.
--@module tman

_G.program = "tman"
_G.version = "0.1.18"

local core = require("src.core.core")
local errmod = require("src.core.errmod")
local builtins = require("src.core.builtin")

---Tman interface.
local function main()
    local cmd = table.remove(arg, 1) or "help"

    for _, builtin in pairs(builtins) do
        if cmd == builtin.name then
            core.setup(builtin.setup_level)
            return builtin.command()
        end
    end

    return core.die(1, errmod.get(errmod.EBCNON), cmd)
end

os.exit(main())
