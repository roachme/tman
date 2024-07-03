local git = require("secondary.git")
local taskid = require("core.taskid")
local common = require("core.common")
local getopt = require("posix.unistd").getopt

--- Pack commits in repos for review.
local function builtin_pack()
    local id
    local optstr = "cmp"
    local last_index = 1
    local fcommit = true -- default option
    local fmake, fpush

    for optopt, _, optind in getopt(arg, optstr) do
        if optopt == "?" then
            common.die(1, "unrecognized option\n", arg[optind - 1])
        end

        last_index = optind
        if optopt == "c" then
            fcommit = true
        elseif optopt == "m" then
            fmake = true
        elseif optopt == "p" then
            fpush = true
        end
    end

    id = arg[last_index] or taskid.getcurr()

    if not id then
        common.die(1, "no current task\n", "")
    end
    if not taskid.exist(id) then
        common.die(1, "no such task ID\n", id)
    end
    if not git.branch_exists(id) then
        common.die(1, "task branch doesn't exist\n", "REPONAME")
    end

    if fpush then
        print("push branch to remote repo: under development")
    elseif fmake then
        print("run commands from the Makefile: under development")
    elseif fcommit then
        print("create commits")
        git.commit_create(id)
    end
    return 0
end

return builtin_pack
