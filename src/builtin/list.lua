local env = require("core.env")
local taskid = require("core.taskid")
local taskunit = require("core.taskunit")
local common = require("core.common")
local help = require("secondary.help")
local getopt = require("posix.unistd").getopt
local config = require("secondary.config")

--- List all task IDs.
-- Default: show only active task IDs.
local function builtin_list()
    local cmdname = "list"
    local active = true
    local completed = false
    local optstring = "Aach"
    local keyhelp
    local envname
    local last_index = 1

    for optopt, _, optind in getopt(arg, optstring) do
        if optopt == "?" then
            common.die(1, "unrecognized option\n", arg[optind - 1])
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

    envname = arg[last_index] or env.getcurr()
    if not envname then
        -- nothing to show, terminate here.
        return 0
    elseif not env.exists(envname) then
        return common.die(1, "no such environment name\n", envname)
    end

    taskid.init(config.core.refs.ids)
    local curr = taskid.getcurr(envname)
    local prev = taskid.getprev(envname)

    if (active or (not active and not completed)) and curr then
        local desc = taskunit.get(envname, curr, "desc")
        print(("* %-10s %s"):format(curr, desc))
    end
    if (active or (not active and not completed)) and prev then
        local desc = taskunit.get(envname, prev, "desc")
        print(("- %-10s %s"):format(prev, desc))
    end

    for i = 1, taskid.size() do
        local item = taskid.getidx(envname, i)

        if item.envname == envname then
            local desc = taskunit.get(envname, item.id, "desc")
            if item.id ~= curr and item.id ~= prev then
                if item.status == taskid.status.ACTV and active then
                    print(("a %-10s %s"):format(item.id, desc))
                elseif item.status == taskid.status.COMP and completed then
                    print(("c %-10s %s"):format(item.id, desc))
                end
            end
        end
    end
    return 0
end

return builtin_list
