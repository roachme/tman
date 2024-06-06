--- Task manager.
-- Simplify workflow when working with many repos.
-- @module TMan

-- Tman main components.
local git = require("git")
local core = require("core")
local struct = require("struct")
local taskid = require("taskid")
local taskunit = require("taskunit")

-- Tman misc components.
local help = require("misc.help")
local getopt = require("posix.unistd").getopt

local errcodes = {
    ok = 0,
    not_inited = 1,
    corrupted = 2,
    command_failed = 2,
    command_not_found = 3,
}

--[[
Erorr codes:
    0 - OK
    1 - structure not inited
    2 - structure corrupted
    3 - command failed to execute
    4 - command not found
]]

-- Private functions: start --

--- Check ID is passed and exists in database.
-- @param id task ID
-- @return true on success, otherwise false
local function _checkid(id)
    id = id or taskid.getcurr()
    if not id then
        io.stderr:write("no current task\n")
        return false
    end
    if not taskid.exist(id) then
        io.stderr:write(("'%s': no such task ID\n"):format(id))
        return false
    end
    return true
end

-- Private functions: end --

-- Public functions: start --

--- Add a new task.
-- @param id task ID
-- @param tasktype task type: bugfix, hotfix, feature. Default: bugfix
-- @param prio task priority: lowest, low, mid, high, highest. Default: mid
-- @treturn true if new task unit is created, otherwise false
local function tman_add(id, tasktype, prio)
    prio = prio or "mid"
    tasktype = tasktype or "bugfix"

    if not id then
        io.stderr:write("task ID required\n")
        os.exit(1)
    end
    if not taskid.add(id) then
        io.stderr:write(("'%s': already exists\n"):format(id))
        os.exit(1)
    end
    if not taskunit.add(id, tasktype, prio) then
        io.stderr:write("could not create new task unit\n")
        taskid.del(id)
        os.exit(1)
    end
    if not struct.create(id) then
        io.stderr:write("could not create new task structure\n")
        taskid.del(id)
        taskunit.del(id)
        os.exit(1)
    end
    if not git.branch_create(id) then
        io.stderr:write("could not create new branch for a task\n")
        taskid.del(id)
        taskunit.del(id)
        struct.delete(id)
        os.exit(1)
    end
    return 0
end

--- Switch to another task.
-- @param id task ID
local function tman_use(id)
    if not _checkid(id) then
        os.exit(1)
    end
    if taskid.getcurr() == id then
        io.stderr:write(("'%s': already in use\n"):format(id))
        os.exit(1)
    end
    if not git.branch_switch(id) then
        io.stderr:write("repo has uncommited changes\n")
        os.exit(1)
    end
    taskid.setcurr(id)
    return 0
end

--- Switch to previous task.
local function tman_prev()
    local prev = taskid.getprev()

    if not prev then
        io.stderr:write("no previous task\n")
        os.exit(1)
    end
    if not git.branch_switch(prev) then
        io.stderr:write("repo has uncommited changes\n")
        os.exit(1)
    end
    taskid.swap()
    return 0
end

--- List all task IDs.
-- Default: show only active task IDs.
-- @param opt list option
local function tman_list()
    local active = true
    local completed = false
    local optstring = "Aac"

    for optopt, _, optind in getopt(arg, optstring) do
        if optopt == "?" then
            local errmsg = "unrecognized option '%s'\n"
            io.stderr:write(errmsg:format(arg[optind - 1]))
            return 0
        end
        if optopt == "A" then
            print("All tasks:")
            active = true
            completed = true
        elseif optopt == "a" then
            print("Active tasks:")
            -- use default flags
        elseif optopt == "c" then
            print("Completed tasks:")
            active = false
            completed = true
        end
    end
    taskid.list(active, completed)
    return 0
end

--- Show task unit metadata.
-- @param id task ID
local function tman_cat(id)
    id = id or taskid.getcurr()

    if not id then
        os.exit(1)
    elseif not taskid.exist(id) then
        io.stderr:write(("'%s': no such task ID\n"):format(id))
        os.exit(1)
    end
    taskunit.show(id)
end

--- Amend task unit.
-- @param opt option
-- @param id task ID
local function tman_set(opt, id)
    id = id or taskid.getcurr()

    if not _checkid(id) then
        os.exit(1)
    end

    if opt == "-d" then
        io.write(("New description (%s): "):format(id))
        local newdesc = io.read("*l")

        if not git.branch_switch(id) then
            return 1
        end
        if not taskunit.amend_desc(id, newdesc) then
            return 1
        end
        git.branch_rename(id)
        return 0
    elseif opt == "-p" then
        local prio = taskunit.getunit(id, "prio")
        io.write(("New priority (%s): "):format(id))
        local newprio = io.read("*l")

        if newprio == prio then
            io.stderr:write("error: it's the same priority\n")
            return 1
        end

        taskunit.amend_prio(id, newprio)

        -- in case prio is part of the branch name.
        if not git.branch_switch(id) then
            return 1
        end
        git.branch_rename(id)
        return 0
    elseif opt == "-i" then
        io.write(("New task ID (%s): "):format(id))
        local newid = io.read("*l")

        if id == newid then
            io.stderr:write("error: it's the same task ID\n")
            return 1
        elseif taskid.exist(newid) then
            io.stderr:write("error: task ID already exists\n")
            return 1
        end

        if not git.branch_switch(id) then
            return 1
        end
        if not taskunit.amend_id(id, newid) then
            return 1
        end
        -- roachme: FIXME: you can't change this order.
        -- It's ok, but not obvious.
        taskid.del(id)
        taskid.add(newid)
        struct.rename(id, newid)
        git.branch_rename(newid)
        return 0
    elseif opt == "-l" then
        io.write(("New task link (%s): "):format(id))
        local newlink = io.read("*l")
        taskunit.amend_link(id, newlink)
        return 0
    elseif not opt then
        io.stderr:write("option missing\n")
    else
        io.stderr:write(("'%s': no such option\n"):format(opt))
    end
end

--- Update git repos.
-- @param cmd command
local function tman_update(cmd)
    local id = taskid.getcurr()

    if not id then
        io.stderr:write("no current task\n")
        os.exit(1)
    end

    -- create task structure if needed
    struct.create(id)

    -- create git branch if needed
    git.branch_create(id)

    -- update active repos
    local active_repos = git.branch_ahead(id)
    taskunit.amend_repos(id, active_repos)

    -- switch to task branch, that's it. Default option.
    if not cmd or cmd == "task" then
        -- roachme: failes if repo's uncommited changes
        git.branch_switch(id)
        return errcodes.ok
    elseif cmd == "repo" then
        if not git.branch_switch_default() then
            return errcodes.command_failed
        end
        git.branch_update(true)
        git.branch_switch(id)
        git.branch_rebase()
        return errcodes.ok
    end
    local errmsg = "%s: update: command not found '%s'\n"
    io.stderr:write(errmsg:format(help.progname, cmd))
    io.stderr:write("Commands: [task|repo]\n")
    return errcodes.command_failed
end

--- Delete task.
-- @param id task ID
local function tman_del(id)
    id = id or taskid.getcurr()

    if not _checkid(id) then
        os.exit(1)
    end

    taskunit.show(id, "desc")
    io.write("Do you want to continue? [Yes/No] ")
    local confirm = io.read("*line")
    if confirm ~= "Yes" then
        print("deletion is cancelled")
        os.exit(1)
    end

    git.branch_delete(id)
    taskunit.del(id)
    taskid.del(id)

    -- roachme: make it pretty and easire to read.
    -- switch back to current task (if exists)
    local curr = taskid.getcurr()
    if curr then
        git.branch_switch(curr)
    end

    -- delete task dir at the end, cuz it causes error for tman.sh
    struct.delete(id)
    return 0
end

--- Move current task to done status.
-- @param id task id
-- roachme: It moves to ACTV, COMP status.
local function tman_done(id)
    local retcode
    local curr = taskid.getcurr()
    local prev = taskid.getprev()
    id = id or curr

    if id ~= "under development" then
        print("under development. Gotta check that task branch is merged")
        return 2
    end

    if not _checkid() then
        os.exit(1)
    end

    -- provide retcode for shell command
    if id == curr then
        retcode = 0
    elseif id == prev then
        retcode = 1
    else
        retcode = 2
    end

    if not git.branch_switch_default() then
        io.stderr:write("repo has uncommited changes\n")
        os.exit(1)
    end

    -- roachme: if task's done delete git branch,
    --          MAYBE task dir as well (nah, leave it)
    --          BUT make sure task branch's merged into default branch

    taskid.move(id, taskid.status.COMP)
    return retcode
end

--- Config util for your workflow
-- @param subcmd subcommand
local function tman_config(subcmd)
    if not subcmd then
        return core.showconf()
    end
end

--- Get special task ID's ID.
-- @param idtype task id
local function tman_get(idtype)
    idtype = idtype or "curr"

    if idtype == "curr" then
        print(taskid.getcurr() or "")
    elseif idtype == "prev" then
        print(taskid.getprev() or "")
    else
        local errmsg = "err: no such ID type '%s'\n"
        io.stderr:write(errmsg:format(idtype or "no idtype"))
    end
end

local function tman_test(id)
    id = id or taskid.getcurr()

    if not _checkid() then
        os.exit(1)
    end

    if git.branch_merged(id) then
        print("ok: all repo branches are merged")
    else
        print("err: not all repo branches are merged")
    end
end

-- Public functions: end --

--- Util interface.
local function main()
    local cmd = arg[1] or "help"
    local corecheck = core.check()

    -- posix getopt does not let permutations as GNU version
    table.remove(arg, 1)

    if corecheck == 1 and cmd ~= "init" then
        io.stderr:write("tman: structure not inited\n")
        return 1
    elseif corecheck == 2 and cmd ~= "init" then
        io.stderr:write("tman: structure corrupted\n")
        return 1
    end

    if cmd == "init" then
        return core.init()
    elseif cmd == "add" then
        return tman_add(arg[1], arg[2], arg[3])
    elseif cmd == "set" then
        return tman_set(arg[1], arg[2])
    elseif cmd == "use" then
        return tman_use(arg[1])
    elseif cmd == "cat" then
        return tman_cat(arg[1])
    elseif cmd == "del" then
        return tman_del(arg[1])
    elseif cmd == "list" then
        return tman_list()
    elseif cmd == "update" then
        return tman_update(arg[1])
    elseif cmd == "done" then
        return tman_done(arg[1])
    elseif cmd == "get" then
        return tman_get(arg[1])
    elseif cmd == "prev" then
        return tman_prev()
    elseif cmd == "config" then
        return tman_config(arg[1])
    elseif cmd == "test" then
        return tman_test(arg[1])
    elseif cmd == "help" then
        return help.usage(arg[1])
    elseif cmd == "ver" then
        print(("%s version %s"):format(help.progname, help.version))
        return 0
    end
    -- error: command not found. Show usage.
    return help.usage(cmd)
end

os.exit(main())
