local common = {}

local prefix = ""

common.struct = {
    prefix = { isdir = false, iscore = true, path = prefix },
    dbdir = { isdir = false, iscore = true, path = prefix .. "/.tman/" },
    units = { isdir = true, iscore = true, path = prefix .. "/.tman/units/" },
    refs = { isdir = true, iscore = true, path = prefix .. "/.tman/refs/" },
    tasks = { isdir = true, iscore = false, path = prefix .. "/tasks/" },
    envs = { isdir = false, iscore = true, path = prefix .. "/.tman/refs/envs" },
    curr = { isdir = false, iscore = true, path = prefix .. "/.tman/curr" },
    ids = { isdir = false, iscore = true, path = prefix .. "/.tman/refs/ids" },
}

return common
