local core = require("src.core.core")
local help = require("src.aux.help")
local getopt = require("posix.unistd").getopt

local function builtin_sync()
    local curr = core.getcurr()

    if not curr.curr then
        return 1
    end

    local newdir = core.prefix .. "/tasks/" .. curr.env .. "/" .. curr.curr
    print(newdir)
    return 0
end

return builtin_sync
