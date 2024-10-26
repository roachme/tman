local core = require("src.core.core")
local errmod = require("src.core.errmod")

local git = require("src.plugin.git.git")
local struct = require("src.plugin.struct.struct")

---Call plugins.
local function builtin_pgn()
    local pgnname = table.remove(arg, 1)

    if pgnname == "git" then
        git.exec()
    elseif pgnname == "struct" then
        struct.exec()
    elseif not pgnname then
        core.die(1, errmod.EPMIS, "pgn")
    else
        core.die(1, errmod.EPNON, pgnname)
    end
end

return builtin_pgn
