local core = require("core.core")
local help = require("aux.help")
local getopt = require("posix.unistd").getopt

local function builtin_move()
    local envname, id
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

    print("under development")
    os.exit(0)
    id = arg[last_index]
    envname = arg[last_index + 1]
    core.move(id, envname)
    print("envname", envname)
end

return builtin_move
