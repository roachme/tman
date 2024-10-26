local core = require("src.core.core")
local getopt = require("posix.unistd").getopt

---Add a new environment.
local function _env_add()
    local desc, envname
    local optstr = "d:h"
    local last_index = 1

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            return core.die(1, "unrecognized option", arg[optind - 1])
        end

        last_index = optind
        if optopt == "d" then
            desc = optarg
        end
    end

    envname = arg[last_index]
    core.env_add(envname, desc)
    return 0
end

---Delete an environment.
local function _env_del()
    local envname
    local optstr = "f:h"
    local last_index = 1
    local force = false

    for optopt, _, optind in getopt(arg, optstr) do
        if optopt == "?" then
            return core.die(1, "unrecognized option", arg[optind - 1])
        end

        last_index = optind
    end

    if not force then
        io.write("Ya fuckin' sure? [Fucking no | yes]? ")
        local confirm = io.read("*line")
        if string.lower(confirm or "") ~= "yes" then
            print("deletion is cancelled")
            os.exit(1)
        end
    end

    envname = arg[last_index]
    core.env_del(envname)
    return 0
end

---Switch to previous environment.
local function _env_prev()
    core.env_prev()
    return 0
end

---Switch to environment.
local function _env_use()
    local envname = table.remove(arg, 1)
    core.env_switch(envname)
    return 0
end

---List environments.
local function _env_list()
    local envlist = core.env_list()
    local statuses = { "*", "^", "+", "-" }

    table.sort(envlist, function(a, b)
        return a.status < b.status
    end)

    for _, item in pairs(envlist) do
        local fmt = "%s %-10s"
        local mark = statuses[item.status + 1]
        print(fmt:format(mark, item.name))
    end
    return 0
end

--- Define or display task environments.
local function builtin_env()
    local cmd = table.remove(arg, 1) or "list"
    local envcmds = {
        add = _env_add,
        del = _env_del,
        list = _env_list,
        prev = _env_prev,
        use = _env_use,
    }

    for name, func in pairs(envcmds) do
        if cmd == name then
            return func()
        end
    end

    core.die(1, "no such environment command", cmd)
    return 0
end

return builtin_env
