local env = require("core.env")
local git = require("secondary.git")
local taskid = require("core.taskid")
local config = require("secondary.config")
local common = require("core.common")
local getopt = require("posix.unistd").getopt

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
            common.die(1, "unrecognized option\n", arg[optind - 1])
        end

        last_index = optind
        if optopt == "c" then
            fcommit = true
        elseif optopt == "m" then
            fmake = true
        end
    end

    if not envname then
        return common.die(1, "no current env", "env")
    end

    taskid.init(config.core.ids)
    id = arg[last_index] or taskid.getcurr(envname)

    if not id then
        common.die(1, "no current task\n", "")
    end
    if not taskid.exists(envname, id) then
        common.die(1, "no such task ID\n", id)
    end
    if not git.branch_exists(id) then
        common.die(1, "task branch doesn't exist\n", "REPONAME")
    end

    if fmake then
        print("run commands from the Makefile: under development")
    elseif fcommit then
        print("create commits")
        git.commit_create(id)
    end
    return 0
end

return builtin_pack
