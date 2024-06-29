local shell = {}

local prefix

function shell.getenv()
    local def_envname = "work"
    local res
    local f = io.open(prefix .. "/env")

    if not f then
        return def_envname
    end
    res = f:read("*a")
    if res == "" then
        res = def_envname
    end
    f:close()
    return res
end

function shell.setenv(val)
    local fname = prefix .. "/env"
    local f = io.open(fname, "w")

    if not f then
        return false
    end
    f:write(val)
    f:close()
    return true
end

function shell.getcurr()
    local res
    local f = io.open(prefix .. "/curr")

    if not f then
        return nil
    end
    res = f:read("*a")
    f:close()
    return res
end

function shell.setcurr(val)
    local f = io.open(prefix .. "/curr", "w")

    if not f then
        return false
    end
    f:write(val)
    f:close()
    return true
end

function shell.init(sh_prefix)
    prefix = sh_prefix
end

return shell
