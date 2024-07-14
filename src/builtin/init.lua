local core = require("core.core")

local function tman_init()
    if not core.create() then
        core.die(1, "could not create core structure", "init")
    end

    return 0
end

return tman_init
