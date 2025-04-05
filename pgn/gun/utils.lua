--- Platfor dependent stuff and general util functions.
-- @module utils

local posix = require("posix")

local utils = {}

---Create directory.
---@param dirname string
---@return boolean
function utils.mkdir(dirname)
    local res = os.execute(("mkdir -p %s"):format(dirname))
    if res == 0 or res == true then
        return true
    end
    return false
end

---Remove file or directory.
---@param name string
---@return boolean
function utils.rm(name)
    local res = os.execute(("rm -rf %s"):format(name))
    if res == 0 or res == true then
        return true
    end
    return false
end

---Create file.
---@param fname string
---@return boolean
function utils.touch(fname)
    local res = os.execute(("touch %s"):format(fname))
    if res == 0 or res == true then
        return true
    end
    return false
end

---Create symlink.
---@param target string
---@param linkname string
---@param soft boolean?
---@return boolean
function utils.link(target, linkname, soft)
    soft = soft or true
    return posix.link(target, linkname, soft)
end

---Check that file or directory exists.
---@param fname string
---@return boolean
function utils.access(fname)
    if posix.access(fname) then
        return true
    end
    return false
end

---Rename file/directory.
---@param oldname string
---@param newname string
---@return boolean
function utils.rename(oldname, newname)
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
function utils.exec(cmd)
    local res = os.execute(cmd)
    if res == 0 or res == true then
        return true
    end
    return false
end

---Generate unit filename.
---@param prjname string
---@param id string
---@return string
function utils.genname(prjname, id)
    return prjname .. "/" .. id
end

---Get user input.
---@param prompt string
---@return string
function utils.get_input(prompt)
    io.write(prompt, ": ")
    local res = io.read("*line")
    res = string.gsub(res, "^%l", string.upper) -- capitalize first letter
    res = string.gsub(res, "%s+$", "") -- remove trailing whitespace
    return res
end

---Return table size
---@param tab table
---@return number
function table.size(tab)
    local size = 0

    if type(tab) ~= "table" then
        return size
    end

    for _, _ in pairs(tab) do
        size = size + 1
    end

    return size
end

return utils
