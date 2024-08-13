local core = require("core.core")
local getopt = require("posix.unistd").getopt

---Set task unit value.
local function builtin_set()
    local envname, id
    local last_index = 1
    local optstr = "d:i:p:t:"
    local options = { }

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            core.die(1, "unrecognized option", arg[optind - 1])
        end

        last_index = optind
        if optopt == "d" then
            options.desc = optarg
        elseif optopt == "i" then
            options.id = optarg
        elseif optopt == "p" then
            options.prio = optarg
        elseif optopt == "t" then
            options.type = optarg
        end
    end

    id = arg[last_index]
    envname = arg[last_index + 1]
    for key, val in pairs(options) do
        core.id_set(envname, id, key, val)
    end
    return 0
end

return builtin_set
