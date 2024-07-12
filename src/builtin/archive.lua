local core = require("core.core")
local help = require("secondary.help")
local core = require("core.core")
local getopt = require("posix.unistd").getopt

--- Backup and restore.
local function tman_archive()
    local optstr = "Rb:hr:"
    local include_repo = false
    local backup_file, restore_file
    local cmdname = "archive"
    local keyhelp

    for optopt, optarg, optind in getopt(arg, optstr) do
        if optopt == "?" then
            core.die(1, "unrecognized option\n", arg[optind - 1])
        end

        if optopt == "R" then
            print("repo included")
            include_repo = true
        elseif optopt == "b" then
            print("backup")
            backup_file = optarg
        elseif optopt == "r" then
            print("restore")
            restore_file = optarg
        elseif optopt == "h" then
            keyhelp = true
        end
    end

    if keyhelp then
        help.usage(cmdname)
        return 0
    end

    if backup_file and restore_file then
        core.die(1, "backup and restore options can't be used together\n", "")
    end

    if backup_file then
        core.backup(backup_file, include_repo)
    elseif restore_file then
        core.restore(restore_file)
    end
    return 0
end

return tman_archive
