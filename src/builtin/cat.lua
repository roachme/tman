local env = require("core.env")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local common = require("core.common")
local help = require("secondary.help")
local getopt = require("posix.unistd").getopt
local config = require("secondary.config")

--- Show task unit metadata.
local function builtin_cat()
    local id
    local last_index = 1
    local optstr = "hk:"
    local key, keyhelp
    local cmdname = "cat"
    local envname = env.getcurr()

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            common.die(1, "unrecognized option\n", arg[optind - 1])
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

    taskid.init(config.core.refs.ids)

    id = arg[last_index] or taskid.getcurr(envname)
    if not id then
        common.die(1, "no current task ID\n", "")
    elseif not taskid.exists(envname, id) then
        common.die(1, "no such task ID\n", id)
    end

    if not taskunit.cat(envname, id, key) then
        if key then
            common.die(1, "no such key\n", key)
        end
    end
    return 0
end

return builtin_cat
