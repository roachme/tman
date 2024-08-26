local errno = nil

local errmod = {
    -- system stuff
    ENINT = "tman not inited",
    ECINT = "could not init module",

    -- task environment
    EEADD = "could not add new environment",
    EECUR = "no current environment set",
    EEDEL = "could not delete environment",
    EEEXT = "environment already exists",
    EEILL = "illegal environment name",
    EENON = "no such environment name",
    EEPRS = "could not switch to previous environment",
    EEPRV = "no previous environment set",
    EEREQ = "environment name required",
    EESET = "could not set current environment",
    EESWT = "could not switch to environment",

    -- task id
    EIADD = "could not add new task id",
    EICUR = "no current task id set",
    EIDEL = "could not delete task id",
    EIEXT = "task id already exists",
    EIIIL = "illegal task id",
    EIDIL = "illegal task description",
    EINON = "no such task id",
    EIPRS = "could not switch to previous task id",
    EIPRV = "no previous task id set",
    EIREQ = "task id required",
    EISET = "could not set current task id",
    EISWT = "could not switch to task id",

    -- switch feature
    EESPRV = "could not switch to previous environment",
    EISPRV = "could not switch to previous task",

    -- unit stuff
    EEUADD = "could not add unit environment",
    EEUDEL = "could not delete unit environment",
    EEUSET = "could not set unit value",

    -- task directory
    EETADD = "could not create task directory",
    EETDEL = "could not delete task directory",
    EETREN = "could not rename task directory",

    -- builtins
    EBCNON = ("no such command. Try '%s help' for more info"):format(program),
    EBKNON = "no such key",

    -- plugin
    EPMIS = "plugin name missing",
    EPNON = "no such plugin",
    EPKNON = "no such plugin command",
}

---Get error description.
---@param desc string?
---@return string
function errmod.get(desc)
    desc = errno or desc
    return desc or ""
end

---Set error.
---@param desc string
---@return false
function errmod.set(desc)
    errno = desc
    return false
end

return errmod
