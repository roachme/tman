local env = require("core.env")
local git = require("secondary.git")
local taskid = require("core.taskid")
local struct = require("secondary.struct")
local taskunit = require("core.taskunit")
local common = require("core.common")
local getopt = require("posix.unistd").getopt
local config = require("secondary.config")

local envname

--- Set task description.
-- @param id task ID
-- @param newdesc new description
-- @return on success - 0
-- @return on failure - error code
local function _set_desc(id, newdesc)
    git.branch_switch(id)
    -- roachme: the only reasons why it might fail
    -- 1. Dir doesn't exist.
    -- 2. Has no permition.
    -- 3. Hardware isssue.
    -- core.lua gotta check that out, so we ain't gotta check, just do it.
    if not taskunit.set(envname, id, "desc", newdesc) then
        return 1
    end
    git.branch_rename(id)
    return 0
end

--- Set task ID.
-- @param id task ID
-- @param newid new task ID
-- @return on success - 0
-- @return on failure - error code
local function _set_id(id, newid)
    local curr = taskid.getcurr(envname)

    if id == newid then
        common.die(1, "the same task ID\n", newid)
    elseif taskid.exists(envname, newid) then
        common.die(1, "task ID already exists\n", newid)
    end

    git.branch_switch(id)
    if not taskunit.set(envname, id, "id", newid) then
        return 1
    end
    -- roachme: FIXME: you can't change this order.
    -- It's ok, but not obvious.
    taskid.del(envname, id)
    taskid.add(envname, newid)

    -- mark current task as current back again.
    if curr and curr ~= id then
        taskid.setcurr(envname, curr)
    end

    struct.rename(common.genname(envname, id), common.genname(envname, newid))
    git.branch_rename(newid)
    return 0
end

--- Set task link.
-- @param id task ID
-- @param newlink new task link
local function _set_link(id, newlink)
    taskunit.set(envname, id, "link", newlink)
    return 0
end

--- Set task priority.
-- @param id task ID
-- @param newprio new priority
local function _set_prio(id, newprio)
    local prio = taskunit.get(envname, id, "prio")

    if newprio == prio then
        common.die(1, "the same priority\n", newprio)
    end
    if not taskunit.set(envname, id, "prio", newprio) then
        return 1
    end
    return 0
end

--- Set task type.
-- @param id task ID
-- @param newtype new type
local function _set_type(id, newtype)
    taskunit.set(envname, id, "type", newtype)
    git.branch_rename(id)
    return 0
end

--- Amend task unit.
-- roachme:FIXME: switches task even when I change random task's unit.
local function tman_set()
    local id
    local last_index = 1
    local optstr = "di:l:p:t:"
    local newdesc -- roachme: get rid of this variable
    local options = {
        newid = { arg = nil, func = _set_id },
        newdesc = { arg = nil, func = _set_desc },
        newlink = { arg = nil, func = _set_link },
        newprio = { arg = nil, func = _set_prio },
        newtype = { arg = nil, func = _set_type },
    }
    envname = env.getcurr()

    -- roachme: It'd be better to show what task ID's changing. Maybe?

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            common.die(1, "unrecognized option\n", arg[optind - 1])
        end

        last_index = optind
        if optopt == "d" then
            io.write(("New description (%s): "):format(""))
            newdesc = io.read("*l")
            if not taskunit.check("desc", newdesc) then
                common.die(1, "description has illegal symbols\n", "")
            end
            -- roachme: be careful when delete newdesc variable.
            options.newdesc.arg = newdesc
        elseif optopt == "i" then
            if not taskunit.check("id", optarg) then
                common.die(1, "invalid id\n", optarg)
            end
            options.newid.arg = optarg
        elseif optopt == "l" then
            options.newlink.arg = optarg
        elseif optopt == "p" then
            if not taskunit.check("prio", optarg) then
                common.die(1, "invalid priority\n", optarg)
            end
            options.newprio.arg = optarg
        elseif optopt == "t" then
            if not taskunit.check("type", optarg) then
                common.die(1, "invalid task type\n", optarg)
            end
            options.newtype.arg = optarg
        end
    end

    if not envname then
        common.die(1, "no such env\n", envname or "no envname")
        return
    end

    taskid.init(config.core.ids)
    id = arg[last_index] or taskid.getcurr(envname)
    if not id then
        common.die(1, "no current task ID\n", "")
    end
    if not taskid.exists(envname, id) then
        common.die(1, "no such task ID\n", id)
    end
    if not git.check(id) then
        common.die(1, "errors in repo. Put meaningful desc here\n", "REPONAME")
    end

    -- roachme: error if no arguments're passed

    if options.newid.arg and options.newtype.arg then
        io.stderr:write("BUG: options '-i' and '-t' can't be used togother\n")
        return 1
    end

    -- set values
    for _, item in pairs(options) do
        if item.arg then
            -- no worries, a function exit if there're any errors.
            item.func(id, item.arg)
        end
    end
    return 0
end

return tman_set
