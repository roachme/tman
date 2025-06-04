#!/usr/bin/env lua

package.path = package.path .. ";" .. os.getenv("HOME") .. "/.config/tman/pgns/?.lua"

-- TODO: Fill out default values for config file.
-- This way plugin will not break.

local config = require("gunconfig")
local utils  = require("utils")
local gitlib = require("gitlib")
local getopt = require("posix.unistd").getopt

local gun = {}
local pgname = "gun"
local pgnver = "v0.0.1"

local isdegub = false


-- Default values for all projects
config._common = {
    prefix = "TP",
    dirbase = "repos",
    commitpatt = "[ID] PART: MSG",
    branchpatt = "TYPE/PREFIX-ID_DESC_DATE",
    repos = {},
}


local function showtable(tab)
    for k, v in pairs(tab) do
        print(k, "-", v)
        if type(v) == 'table' then
            for kk, vv in pairs(v) do
                print("  ", kk, "-", vv)
            end
        end
    end
end

function table.join(tab1, tab2)
    local tab = {}

    for k, v in pairs(tab1) do
        tab[k] = v
    end
    for k, v in pairs(tab2) do
        tab[k] = v
    end
    return tab
end


local function elog(...)
    io.stderr:write(string.format("%s: ", pgname))
    for _, val in pairs({...}) do
        io.stderr:write(val, " ")
    end
    io.stderr:write('\n')
    return false
end

local function dlog(...)
    if not isdegub then
        return 0
    end
    for _, val in pairs({...}) do
        io.write(val, " ")
    end
    io.write('\n')
    return 0
end

local function load(fname)
    local units = {}
    local f = io.open(fname)

    if not f then
        return {}
    end
    for line in f:lines() do
        local key, val = string.match(line, "(%w*) : (.*)")
        units[key] = val
        dlog("load:", line)
    end
    return units
end

local function save(fname, units)
    local pgnkeys = { "branch" }
    local f = io.open(fname, "w")
    if not f then
        return elog("save: could not open unit file")
    end
    for _, v in pairs(pgnkeys) do
        f:write(v, " : ", units[v], '\n')
        dlog("save: ", v, " : ", units[v], '\n')
    end
    -- FIXME: maybe will cause problems on other Lua versions.
    return f:close() == true
end

--- Setup basic pathes
-- taskdir -- where what project task is located at
-- unitdir -- where unit files are located (system and plugin)
-- repodir -- where downloaded repos are located
local function optgen(basic)
    local prjconf = config[basic.prj] or {}

    basic.taskdir = basic.base .. '/' .. basic.prj .. '/' .. basic.id
    basic.unitdir = basic.taskdir .. '/' .. '.tman'
    basic.sysfile = basic.unitdir .. '/' .. 'unit'

    -- dirs gotta be created
    basic.repodir = basic.base .. '/' .. '.pgn/gun'
    basic.farmdir = basic.taskdir .. '/' .. (prjconf.dirbase or "")
    basic.upgndir = basic.unitdir .. '/' .. 'pgn'
    -- files gotta be created
    basic.pgnfile = basic.upgndir .. '/' .. 'gun'
    return basic
end

local function struct(dirs, files)
    for _, dir in pairs(dirs) do
        dlog("dir", dir)
        if not utils.mkdir(dir) then
            return elog("could not create directory", dir)
        end
    end
    for _, file in pairs(files) do
        dlog("file", file)
        if not utils.touch(file) then
            return elog("could not create file", file)
        end
    end
    return true
end

local function clone(repos, repodir)
    for _, repo in pairs(repos) do
        if not gitlib.repo_clone(repo.link, repo.name, repodir) then
            return elog("could not clone repo:", repo.name)
        end
    end
    return true
end

-- TODO: symlink only the repos defined for specified project, not all of 'em
local function symlink(repos, src, dst)
    for _, repo in pairs(repos) do
        local target = src .. '/' .. repo.name
        local linkname = dst .. '/' .. repo.name
        if not utils.access(linkname) then
            if not utils.link(target, linkname) then
                return elog("could not create symlink:", linkname)
            end
        end
    end
    return true
end

---String separator.
---@param inputstr string
---@param sep string | nil
---@return table
local function pattsplit(inputstr, sep)
    local res = {}

    if sep == nil then
        sep = "%s"
    end
    for str in string.gmatch(inputstr, "([^" .. sep .. "]+)") do
        table.insert(res, str)
    end
    return res
end

local function branch_generate(branchpatt, sysunits)
    local separators = "/_-"
    local sepcomponents = pattsplit(branchpatt, separators)

    for _, item in pairs(sepcomponents) do
        local uitem = sysunits[string.lower(item)]

        -- roachme: HOTFIX: desc: replace whitespace with undrescore
        if item == "DESC" then
            uitem = string.gsub(uitem, " ", "_")
        end
        branchpatt = string.gsub(branchpatt, item, uitem)
    end
    return branchpatt
end

local function _branch_create(repodir, repos, branchname)
    for _, repo in pairs(repos) do
        if gitlib.branch_exist(repo.name, branchname, repodir) then
            -- needed branch exists, do nothing
        elseif gitlib.repo_isuncommited(repo.name, repodir) then
            return elog(string.format("%s: has uncommited changes", repo.name))
        elseif not gitlib.branch_switch(repo.name, repo.branch, repodir) then
            return elog(string.format("%s: could not switch to deafult branch", repo.name))
        elseif not gitlib.branch_create(repo.name, branchname, repodir) then
            return elog(string.format("%s: could not create a branch in repo", repo.name))
        end
    end
    return true
end

local function _branch_rename(repodir, repos, oldbranch, newbranch)
    dlog("[*] rename branch: under development")
    for _, repo in pairs(repos) do
        if not gitlib.branch_rename(repo.name, oldbranch, newbranch, repodir) then
            return elog(string.format("%s: could not rename branch in repo", repo.name))
        end
    end
    return true
end

-- FIXME: creates branches in all repos, even if they're not defined for an project
local function mkbranch(basic, units)
    local repos = units.repos
    local newbranch = branch_generate(units.branchpatt, units)

    if units.branch ~= nil and units.branch ~= newbranch then
        dlog("[*] rename a branch")
        if not _branch_rename(basic.repodir, repos, units.branch, newbranch) then
            return false
        end
    else
        -- TODO: optimize code. Do nothing if branch created and ok.
        dlog("[*] create a branch if needed")
        if not _branch_create(basic.repodir, repos, newbranch) then
            return false
        end
    end

    -- FIXME: rewrites file even if branch name hasn't changed
    units.branch = newbranch
    return save(basic.pgnfile, units)
end

local function branch_delete(basic, repos, pgnunits)
    for _, repo in pairs(repos) do
    end
end

local function switch(basic, repos, repodir)
    local pgnunits = load(basic.pgnfile)
    local branchname = pgnunits.branch

    for _, repo in pairs(repos) do
        if not gitlib.branch_switch(repo.name, branchname, repodir) then
            return elog(string.format("%s: could not switch", repo.name))
        end
    end
    return true
end

local function pullall(repos, repodir, branchname)
    for _, repo in pairs(repos) do
        if not gitlib.branch_switch(repo.name, repo.branch, repodir)  then
            return elog(string.format("%s: could not switch to default branch", repo.name))
        elseif not gitlib.branch_pullall(repo.name, repodir) then
            return elog(string.format("%s: could not pull from remote repo", repo.name))
        elseif not gitlib.branch_switch(repo.name, branchname, repodir)  then
            return elog(string.format("%s: could not switch to task branch", repo.name))
        end
    end
    return true
end

-- TODO: can't handle rebase/merge conflicts
local function rebase(repos, repodir)
    for _, repo in pairs(repos) do
        if not gitlib.branch_rebase(repo.name, repo.branch, repodir) then
             return elog(string.format("%s: could not rebase. Merge conflict", repo.name))
        end
    end
    return true
end


local function setup(basic)
    local files = { basic.pgnfile }
    local dirs = { basic.repodir, basic.upgndir, basic.farmdir }

    if not struct(dirs, files) then
        return false
    end
    return true
end

local function getunits(basic)
    local sysunits = load(basic.sysfile);
    local pgnunits = load(basic.pgnfile);
    local units = table.join(sysunits, pgnunits)

    units.id = basic.id

    if config[basic.prj] == nil then
        config[basic.prj] = {}
    end

    -- Set values that all projects share (if set any)
    units.repos = config[basic.prj].repos or config._common.repos
    units.prefix = config[basic.prj].prefix or config._common.prefix
    units.dirbase = config[basic.prj].dirbase or config._common.dirbase
    units.commitpatt = config[basic.prj].commitpatt or config._common.commitpatt
    units.branchpatt = config[basic.prj].branchpatt or config._common.branchpatt
    return units
end

function gun.sync(basic)
    local units = getunits(basic)

    if not clone(units.repos, basic.repodir) then
        return 1
    elseif not symlink(units.repos, basic.repodir, basic.farmdir) then
        return 1
    elseif not mkbranch(basic, units) then
        return 1
    elseif not switch(basic, units.repos, basic.repodir) then
        return 1
    end
    return 0
end

function gun.rsync(basic)
    local units = getunits(basic)
    local branchname = branch_generate(units.branchpatt, units)

    if not gun.sync(basic) then
        return 1
    elseif not pullall(units.repos, basic.repodir, branchname) then
        return 1
    elseif not rebase(units.repos, basic.repodir) then
        return 1
    end
    return 0
end

function gun.merge(basic)
    local units = getunits(basic)
    local sysbranch = branch_generate(units.branchpatt, units)
    local branchname = units.branch or sysbranch

    for _, repo in pairs(units.repos) do
        if gitlib.repo_isuncommited(repo.name, basic.repodir) then
            elog(string.format("Cannot delete branch in '%s': has uncommited changes", repo.name));
        elseif not gitlib.branch_switch(repo.name, repo.branch, basic.repodir) then
            elog("could not switch to default branch", repo.name)
        elseif not gitlib.branch_rebase(repo.name, branchname, basic.repodir) then
            elog("could not rebase against default branch", repo.name)
        end
    end
end

function gun.del(basic)
    local units = getunits(basic)
    local sysbranch = branch_generate(units.branchpatt, units)
    local branchname = units.branch or sysbranch

    for _, repo in pairs(units.repos) do
        if gitlib.repo_isuncommited(repo.name, basic.repodir) then
            elog(string.format("Cannot delete branch in '%s': has uncommited changes", repo.name));
        elseif not gitlib.branch_switch(repo.name, repo.branch, basic.repodir) then
            elog("could not switch to default branch", repo.name)
        elseif branchname then
            if gitlib.branch_exist(repo.name, branchname, basic.repodir) then
                if not gitlib.branch_delete(repo.name, branchname, basic.repodir) then
                    elog("could not delete branch", repo.name)
                end
            end
        end
    end
    return 0
end

function gun.commit(basic)
    local commitdesc = ""
    basic.commitpatt = "tman: MSG" -- TODO: this default somewhere else
    local sysunits = load(basic.sysfile);
    local units = getunits(basic)


    commitdesc = basic.commitpatt:gsub("ID", basic.id)
    commitdesc = commitdesc:gsub("MSG", sysunits.desc)
    -- if not gitlib.repo_clone(repo.link, repo.name, repodir) then
    -- if gitlib.branch_exist(repo.name, branchname, repodir) then
    for _, repo in pairs(units.repos) do
        print(repo.name)
        gitlib.commit_create(repo.name, commitdesc, basic.repodir)
    end
end


function gun.show(basic)
    local pgnunits = load(basic.pgnfile)
    for key, val in pairs(pgnunits) do
        print(string.format("%s : %s", key, val))
    end
    return 0
end

function gun.help(basic)
    print([[
Usage: gun [OPITONS]... COMMAND [ARGS]...

Here is list of commands:
  commit    - commit changes via commit pattern
  del       - delete branch
  help      - show this help message and exit
  merge     - merge to default branch (rebase, merge, cherry-pick)
  rsync     - remote sync with remote git repo
  show      - show plugin units
  sync      - switch to task branch
  ver       - show version and exit ]])
end

function gun.ver(basic)
    print(string.format("%s: %s", pgname, pgnver))
end

local function main()
    local cmd = nil
    local basic = {}
    local lastidx = 1
    local bincmds = {
        { name = "commit", func = gun.commit },
        { name = "del",    func = gun.del    },
        { name = "help",   func = gun.help   },
        { name = "merge",  func = gun.merge  },
        { name = "rsync",  func = gun.rsync  },
        { name = "show",   func = gun.show   },
        { name = "sync",   func = gun.sync   },
        { name = "ver",    func = gun.ver    },
    }

    for optopt, optarg, optind in getopt(arg, ":T:p:i:") do
        if optopt == "?" then
            return elog("unrecognized option", arg[optind - 1])
        end
        lastidx = optind
        if optopt == "T" then
            basic.base = optarg
        elseif optopt == "p" then
            basic.prj = optarg
        elseif optopt == "i" then
            basic.id = optarg
        end
    end

    cmd = table.remove(arg, lastidx) or "sync"
    basic = optgen(basic)

    -- TODO: setup
    if not setup(basic) then
        return 1
    end

    for _, bin in pairs(bincmds) do
        if cmd == bin.name then
            return bin.func(basic)
        end
    end
    elog(string.format("cannot find '%s': no such command", cmd))
    return 1;
end

os.exit(main())
