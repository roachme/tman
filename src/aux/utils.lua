--- Platfor dependent stuff and general util functions.
-- @module utils

local posix = require("posix")

---Create directory.
---@param dirname string
---@return boolean
local function create_dir(dirname)
    local res
    os.execute(("mkdir -p %s"):format(dirname))
    if res == 0 or res == true then
        return true
    end
    return false
end

---Remove file or directory.
---@param name string
---@return boolean
local function remove_dir(name)
    local res = os.execute(("rm -rf %s"):format(name))
    if res == 0 or res == true then
        return true
    end
    return false
end

---Create file.
---@param fname string
---@return boolean
local function create_file(fname)
    local res = os.execute(("touch %s"):format(fname))
    if res == 0 or res == true then
        return true
    end
    return false
end

---Create symlink.
---@param target string
---@param linkname string
---@param soft boolean
---@return boolean
local function create_symlink(target, linkname, soft)
    soft = soft or true
    return posix.link(target, linkname, soft)
end

---Check that file or directory exists.
---@param fname string
---@return boolean
local function file_exists(fname)
    if posix.access(fname) then
        return true
    end
    return false
end

---Rename file/directory.
---@param oldname string
---@param newname string
---@return boolean
local function util_rename(oldname, newname)
    if not oldname then
        print("util:rename:error: no oldname")
        return false
    end
    if not newname then
        print("util:rename:error: no newname")
        return false
    end
    local res = os.execute(("mv %s %s"):format(oldname, newname))
    if res == 0 or res == true then
        return true
    end
    return false
end

---Execute system command.
---@param cmd string
---@return boolean
local function util_exec(cmd)
    local res = os.execute(cmd)
    if res == 0 or res == true then
        return true
    end
    return false
end

return {
    rm = remove_dir,
    link = create_symlink,
    exec = util_exec,
    mkdir = create_dir,
    touch = create_file,
    access = file_exists,
    rename = util_rename,
}
