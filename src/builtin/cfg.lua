local core = require("core.core")
local getopt = require("posix.unistd").getopt

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
        print("set base & core values: under development", vbase)
    elseif finstall then
        print("set install value (under development)", vinstall)
    elseif fshow then
        print("show config: under development")
    end
    return 0
end

return tman_config
