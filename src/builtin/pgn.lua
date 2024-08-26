local core = require("core.core")
local errmod = require("core.errmod")
local git = require("plugin.git.git")

local function pgn_git()
    local cmd = table.remove(arg, 1) or "sync"
    local envname = core.getcurr().env
    local taskid = core.getcurr().curr

    if not envname then
        core.die(1, errmod.EECUR, "env")
    elseif not taskid then
        core.die(1, errmod.EICUR, "id")
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
        core.die(1, errmod.EPKNON, cmd)
    end
end

local function builtin_pgn()
    local pgnname = table.remove(arg, 1)

    if pgnname == "git" then
        pgn_git()
    elseif not pgnname then
        core.die(1, errmod.EPMIS, "pgn")
    else
        core.die(1, errmod.EPNON, pgnname)
    end
end

return builtin_pgn
