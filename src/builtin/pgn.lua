local core = require("core.core")
local errmod = require("core.errmod")
local git = require("plugin.git.git")

---Call plugins.
local function builtin_pgn()
    local pgnname = table.remove(arg, 1)

    if pgnname == "git" then
        git.exec()
    elseif not pgnname then
        core.die(1, errmod.EPMIS, "pgn")
    else
        core.die(1, errmod.EPNON, pgnname)
    end
end

return builtin_pgn
