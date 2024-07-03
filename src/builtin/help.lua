local help = require("secondary.help")

local function tman_help()
    return help.usage(arg[1])
end

return tman_help
