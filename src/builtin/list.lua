local core = require("core.core")
local help = require("aux.help")
local getopt = require("posix.unistd").getopt

--- List all task ids.
-- Default: show only active task ids.
local function builtin_list()
    local tasks
    local cmdname = "list"
    local active = true
    local completed = false
    local optstring = "Aach"
    local keyhelp
    local last_index = 1
    local statuses = { "*", "^", "+", "-" }

    for optopt, _, optind in getopt(arg, optstring) do
        if optopt == "?" then
            core.die(1, "unrecognized option", arg[optind - 1])
        end

        last_index = optind
        if optopt == "A" then
            active = true
            completed = true
        elseif optopt == "a" then
            active = true
            completed = false
        elseif optopt == "c" then
            active = false
            completed = true
        elseif optopt == "h" then
            keyhelp = true
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 0
    end

    local envname = arg[last_index] or core.env_curr()
    tasks = core.id_list(envname)

    table.sort(tasks, function(a, b)
        return a.status < b.status
    end)

    if envname then
        print(("envname: %s"):format(envname))
    end
    for _, task in pairs(tasks) do
        local fmt = "%s %-10s %s"
        local status = task.status
        local mark = statuses[status + 1]

        if completed and task.status == 3 then
            print(fmt:format(mark, task.id, task.desc))
        elseif active and task.status ~= 3 then
            print(fmt:format(mark, task.id, task.desc))
        end
    end
    return 0
end

return builtin_list
