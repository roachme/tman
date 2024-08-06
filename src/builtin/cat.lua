local core = require("core.core")
local help = require("aux.help")
local getopt = require("posix.unistd").getopt

---Show task unit metadata.
local function builtin_cat()
    local id
    local envname
    local last_index = 1
    local optstr = "hk:"
    local key, keyhelp
    local cmdname = "cat"
    local units

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            core.die(1, "unrecognized option", arg[optind - 1])
        end
        last_index = optind
        if optopt == "k" then
            key = optarg
        elseif optopt == "h" then
            keyhelp = true
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 0
    end

    id = arg[last_index]
    envname = arg[last_index + 1]
    units = core.cat(envname, id)

    if key then
        if not units[key] then
            core.die(1, "no such key", key, units[key])
        end
        print(("%-8s: %s"):format(key, units[key]))
    else
        for ukey, uval in pairs(units) do
            print(("%-8s: %s"):format(ukey, uval))
        end
    end

    return 0
end

return builtin_cat
