--- Terminal task manager.
-- Simplify workflow when working with many repos.
-- @module TMan

local setup = require("core.setup")
local builtin = require("core.builtin")
local help = require("core.help")

--- Util interface.
local function main()
    local cmd = arg[1] or "help"

    -- POSIX getopt() does not let permutations as GNU version.
    table.remove(arg, 1)

    setup.sys() -- setup system stuff

    -- roachme: is it needed or maybe it's better use `tman sync -s' command?
    setup.user() -- setup user stuff like stuff from user.conf

    -- Call a builtin command.
    for name, func in pairs(builtin) do
        if cmd == name then
            return func()
        end
    end

    -- Command not found. Show some help.
    return help.usage(cmd)
end

os.exit(main())
