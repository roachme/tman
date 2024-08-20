local core = require("core.core")
local git = require("plugin.git")

local function pgn_git()
    local cmd = table.remove(arg, 1) or "sync"
    local envname = core.getcurr().env
    local taskid = core.getcurr().curr

    if not envname then
        core.die(1, "no current environtment", "pgn git")
        return false
    elseif not taskid then
        core.die(1, "no current task id", "pgn git")
        return false
    end

    if cmd == "sync" then
        git.sync(envname, taskid)
    elseif cmd == "rsync" then
        git.rsync(envname, taskid)
    elseif cmd == "pr" then
        git.pr(envname, taskid)
    elseif cmd == "cleanup" then
        git.cleanup()
    else
        core.die(1, "no such plugin git command", "pgn")
    end
end

local function builtin_pgn()
    local pgnname = table.remove(arg, 1)

    if pgnname == "git" then
        pgn_git()
    elseif not pgnname then
        core.die(1, "gotta specify plugin name", "pgn")
    else
        core.die(1, "no such plugin", pgnname)
    end
end

return builtin_pgn
