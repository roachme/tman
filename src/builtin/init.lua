local core = require("src.core.core")
local utils = require("src.aux.utils")
local getopt = require("posix.unistd").getopt

local function tman_init()
    local optstr = "b:"

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            return core.die(1, "unrecognized option", arg[optind - 1])
        end
    end

    if not core.create() then
        core.die(1, "could not create core structure", "init")
    end

    local gitcmd = "git -C %s init"
    utils.exec(gitcmd:format(core.prefix))
    return 0
end

return tman_init
