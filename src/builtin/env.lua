local env = require("core.env")
local core = require("core.core")
local common = require("core.common")

--- Define or display task environments.
local function builtin_env()
    local cmd = arg[1] or "list"
    local envname = arg[2]

    if cmd == "add" then
        if not envname then
            common.die(1, "env name is required\n", "no envname")
        end
        if env.exists(envname) then
            common.die(1, "such env name already exists\n", envname)
        end
        env.add(envname, "auto generated description " .. envname)
        --core.init()
    elseif cmd == "curr" then
        print(env.getcurr())
    elseif cmd == "del" then
        envname = envname or env.getcurr()
        print("env: del env", envname)

        io.write("Do you want to continue? [Yes/No] ")
        local confirm = io.read("*line")
        if confirm ~= "Yes" then
            print("deletion is cancelled")
            os.exit(1)
        end
        env.del(envname)
    elseif not cmd or cmd == "list" then
        print("env: list env names")
        env.list()
    elseif cmd == "prev" then
        local prev = env.getprev()
        env.setcurr(prev)
    elseif cmd == "use" then
        if not env.exists(envname) then
            common.die(1, "no such env name\n", envname)
        end
        env.setcurr(envname)
    else
        common.die(1, "no such env command\n", cmd)
    end
    return 0
end

return builtin_env
