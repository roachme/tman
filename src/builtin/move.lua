local core = require("core.core")
local help = require("aux.help")
local getopt = require("posix.unistd").getopt

local function builtin_move()
    local srcenv, dstenv, id
    local optstr = "hf"
    local last_index = 1
    local cmdname = "move"
    local keyhelp = false

    for optopt, _, optind in getopt(arg, optstr) do
        if optopt == "?" then
            return core.die(1, "unrecognized option", arg[optind - 1])
        end

        last_index = optind
        if optopt == "h" then
            keyhelp = true
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 1
    end

    id = arg[last_index]
    dstenv = arg[last_index + 1]
    srcenv = arg[last_index + 2]
    core.id_move(dstenv, srcenv, id)
    return 0
end

return builtin_move
