local core = require("core.core")
local getopt = require("posix.unistd").getopt
local config = require("aux.config")

--- Config util for your workflow
local function tman_config()
    local optstr = "b:i:s"
    local fshow = true -- default option
    local fbase, finstall
    local vbase, vinstall

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            core.die(1, "unrecognized option", arg[optind - 1])
        end
        if optopt == "b" then
            fbase = true
            vbase = optarg
        elseif optopt == "i" then
            finstall = true
            vinstall = optarg
        elseif optopt == "s" then
            fshow = true
        end
    end

    if fbase then
        -- roachme: check that vbase prefix directory can be created.
        --          convert to correct value if it's "." and has "~/"
        config.sset("prefix", vbase)
    elseif finstall then
        print("set install value (under development)", vinstall)
    elseif fshow then
        config.sshow()
    end
    return 0
end

return tman_config
