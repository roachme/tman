local core = require("src.core.core")
--local help = require("aux.help")
--local getopt = require("posix.unistd").getopt

--- Switch to task.
local function tman_use()
    local envname
    local id
    -- roachme: can't use help option cuz tman.sh fails.

    id = arg[1]
    envname = arg[2]
    core.id_switch(envname, id)
    return 0
end

return tman_use
