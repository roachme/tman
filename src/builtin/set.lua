local env = require("core.env")
local git = require("secondary.git")
local taskid = require("core.taskid")
local struct = require("secondary.struct")
local taskunit = require("core.taskunit")
local common = require("core.common")
local getopt = require("posix.unistd").getopt
local config = require("secondary.config")

local envname

---Set task description.
---@param id string
---@param newdesc string
---@return boolean
local function _set_desc(id, newdesc)
    local path = config.aux.code
    local olddesc = taskunit.get(envname, id, "desc")
    local oldbranch = taskunit.get(envname, id, "branch")
    local newbranch

    if olddesc == newdesc then
        common.die(1, "the same task description\n", newdesc)
    end

    -- roachme: the only reasons why it might fail
    -- 1. Dir doesn't exist.
    -- 2. Has no permition.
    -- 3. Hardware isssue.
    -- core.lua gotta check that out, so we ain't gotta check, just do it.
    if not taskunit.set(envname, id, "desc", newdesc) then
        common.die(1, "could not set new task description\n", "desc")
    end

    newbranch = taskunit.get(envname, id, "branch")
    for _, repo in pairs(config.user.repos) do
        if not git.branch_switch(repo.name, oldbranch, path) then
            common.die(1, "could not switch to task branch\n", repo.name)
        end
        if not git.branch_rename(repo.name, oldbranch, newbranch, path) then
            common.die(1, "could not rename task branch\n", repo.name)
        end
    end
    return true
end

---Set task ID.
---@param id string
---@param newid string
---@return boolean
local function _set_id(id, newid)
    local path = config.aux.code
    local newbranch
    local currbranch = taskunit.get(envname, id, "branch")

    for _, repo in pairs(config.user.repos) do
        if git.repo_isuncommited(repo.name, path) then
            common.die(1, "repo has uncommited changes\n", repo.name)
        end
    end

    if id == newid then
        common.die(1, "the same task ID\n", newid)
    elseif taskid.exists(envname, newid) then
        common.die(1, "task id exists\n", newid)
    elseif not taskunit.set(envname, id, "id", newid) then
        common.die(1, "could not set new task id\n", newid)
    end

    newbranch = taskunit.get(envname, id, "branch")

    -- rename task id and its directory.
    taskid.del(envname, id)
    taskid.add(envname, newid)
    struct.rename(common.genname(envname, id), common.genname(envname, newid))

    for _, repo in pairs(config.user.repos) do
        git.branch_rename(repo.name, currbranch, newbranch, path)
    end
    return true
end

---Set task link.
---@param id string
---@param newlink string
---@return boolean
local function _set_link(id, newlink)
    taskunit.set(envname, id, "link", newlink)
    return true
end

---Set task priority.
---@param id string
---@param newprio string
---@return boolean
local function _set_prio(id, newprio)
    local prio = taskunit.get(envname, id, "prio")

    if newprio == prio then
        local errfmt = "'%s' and '%s' are the same task priority\n"
        common.die(1, errfmt, "set", prio, newprio)
    elseif not taskunit.set(envname, id, "prio", newprio) then
        common.die(1, "could not set new priority", newprio)
    end
    return true
end

---Set task type.
---@param id string
---@param newtype string
---@return boolean
local function _set_type(id, newtype)
    local path = config.aux.code
    local oldbranch = taskunit.get(envname, id, "branch")
    local currtype = taskunit.get(envname, id, "type")
    taskunit.set(envname, id, "type", newtype)
    local newbranch = taskunit.get(envname, id, "branch")

    if currtype == newtype then
        local errfmt = "'%s' and '%s' are the same task type\n"
        common.die(1, errfmt, "set", currtype, newtype)
        return true
    end

    for _, repo in pairs(config.user.repos) do
        if not git.branch_switch(repo.name, oldbranch, path) then
            common.die(1, "could not switch to task branch\n", repo.name)
        end
        if not git.branch_rename(repo.name, oldbranch, newbranch, path) then
            common.die(1, "could not rename task branch\n", repo.name)
        end
    end
    return true
end

---Set task unit value.
local function builtin_set()
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

    local path = config.aux.code
    for _, repo in pairs(config.user.repos) do
        if git.repo_isuncommited(repo.name, path) then
            return common.die(1, "repo has uncommited changes\n", repo.name)
        end
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

return builtin_set
