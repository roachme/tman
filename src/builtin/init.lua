local core = require("core.core")

local function tman_init()
    return core.create() and 0 or 1
end

return tman_init
