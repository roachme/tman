local core = require("core.core")
local help = require("aux.help")
local getopt = require("posix.unistd").getopt

--- Delete task.
local function builtin_del()
    local id = arg[1]
    local envname
    local cmdname = "del"
    local optstr = "fh"
    local keyhelp
    local force = false
    local last_index = 1

    for optopt, _, optind in getopt(arg, optstr) do
        if optopt == "?" then
            return core.die(1, "unrecognized option", arg[optind - 1])
        end

        last_index = optind
        if optopt == "f" then
            force = true
        elseif optopt == "h" then
            keyhelp = true
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 1
    end

    id = arg[last_index]
    envname = arg[last_index + 1]

    if not force then
        io.write("Do you want to continue? [Yes/No] ")
        local confirm = io.read("*line")
        if confirm ~= "Yes" then
            print("deletion is cancelled")
            os.exit(1)
        end
    end

    core.id_del(envname, id)
    return 0
end

return builtin_del
