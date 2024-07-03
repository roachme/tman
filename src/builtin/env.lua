local env = require("core.env")
local core = require("core.core")
local shell = require("aux.shell")
local common = require("core.common")
local taskid = require("core.taskid")
local config = require("core.config")
--local help = require("core.help")
--local getopt = require("posix.unistd").getopt

env.init(config.sys.fenv)

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
        core.init()


        taskid.init(config.core.ids)
        local curr_id = taskid.getcurr(envname)
        if curr_id then
            shell.setcurr(envname .. ":" .. curr_id)
        else
            shell.setcurr("")
        end


    elseif cmd == "curr" then
        print(env.getcurr())
    elseif cmd == "del" then
        envname = envname or env.getcurr()

        io.write("Do you want to continue? [Yes/No] ")
        local confirm = io.read("*line")
        if confirm ~= "Yes" then
            print("deletion is cancelled")
            os.exit(1)
        end
        env.del(envname)
    elseif not cmd or cmd == "list" then
        env.list()
    elseif cmd == "prev" then
        local prev = env.getprev()

        if not prev then
            return common.die(1, "no previous env\n", "env")
        end

        env.setcurr(prev)
        -- roachme: gotta structure it.
        -- update task as well
        taskid.init(config.core.ids)
        local curr_id = taskid.getcurr(prev)
        if curr_id then
            shell.setcurr(prev .. ":" .. curr_id)
        else
            shell.setcurr("")
        end
    elseif cmd == "use" then
        if not env.exists(envname) then
            common.die(1, "no such env name\n", envname)
        end
        env.setcurr(envname)

        taskid.init(config.core.ids)
        local curr_id = taskid.getcurr(envname)
        shell.setcurr(envname .. ":" .. curr_id)
    else
        common.die(1, "no such env command\n", cmd)
    end
    return 0
end

return builtin_env
