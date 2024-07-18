local env = require("core.env")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local core = require("core.core")
local getopt = require("posix.unistd").getopt
local plugin = require("core.plugin")

--- Synchronize task dir: structure, task status, remote repo.
-- With no options jump to task dir.
-- TODO: if wd util not supported then add its features here. Optioon `-w'.
local function tman_sync()
    local id
    local optstr = "Ggst"
    local fstruct
    local fgit_local
    local fgit_remote
    local ftask
    local last_index = 1
    local envname = env.getcurr()

    for optopt, _, optind in getopt(arg, optstr) do
        if optopt == "?" then
            core.die(1, "unrecognized option", arg[optind - 1])
        end

        last_index = optind
        if optopt == "g" then
            fgit_local = true
        elseif optopt == "G" then
            fgit_remote = true
        elseif optopt == "s" then
            fstruct = true
        elseif optopt == "t" then
            ftask = true
        end
    end

    if not envname then
        return core.die(1, "no current env", "env")
    end

    taskid.init(core.struct.ids.path)

    id = arg[last_index] or taskid.getcurr(envname)
    if not id then
        core.die(1, "no current task id", "")
    elseif not taskid.exists(envname, id) then
        core.die(1, "no such task id", id)
    end


    if not plugin.init(envname) then
        return core.die(1, "could not init plugins", "plugin")
    end

    local branch = taskunit.get(envname, id, "branch")
    if fgit_local then
        if not plugin.git.update_local(id, branch) then
            core.die(1, "could not update local repo", "git")
        end
    end

    if fgit_remote then
        if not plugin.git.update_remote(id, branch) then
            core.die(1, "could not update remote repo", "git")
        end
    end

    if fstruct then
        plugin.struct.create(id)
    end

    if ftask then
        print("sync: task status: under development")
    end

    return 0
end

return tman_sync
