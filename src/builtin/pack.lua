local env = require("core.env")
local taskid = require("core.taskid")
local core = require("core.core")
local getopt = require("posix.unistd").getopt
local plugin = require("core.plugin")



--- Pack commits in repos for review.
local function builtin_pack()
    local id
    local optstr = "cm"
    local last_index = 1
    local fcommit = true -- default option
    local fmake
    local envname = env.getcurr()

    for optopt, _, optind in getopt(arg, optstr) do
        if optopt == "?" then
            core.die(1, "unrecognized option", arg[optind - 1])
        end

        last_index = optind
        if optopt == "c" then
            fcommit = true
        elseif optopt == "m" then
            fmake = true
        end
    end

    if not envname then
        return core.die(1, "no current env", "env")
    end

    taskid.init(core.struct.ids.path)
    id = arg[last_index] or taskid.getcurr(envname)

    if not id then
        core.die(1, "no current task", "")
    end
    if not taskid.exists(envname, id) then
        core.die(1, "no such task id", id)
    end

    if not plugin.init(envname) then
        return core.die(1, "could not init plugin", "plugin")
    end

    if fmake then
        print("run commands from the Makefile: under development")
    elseif fcommit then
        print("create commits")
        plugin.git.commit_create(id)
    end
    return 0
end

return builtin_pack
