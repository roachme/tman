local errno = nil

local errmod = {
    ENINT = "tman not inited",
    ECINT = "could not init module",

    EEREQ = "environment name required",
    EENON = "no such environment name",
    EECUR = "no current environment set",
    EEPRV = "no previous environment set",
    EEPRS = "could not switch to previous environment",
    EEDEL = "could not delete environment",
    EEADD = "could not add new environment",
    EEEXT = "environment already exists",
    EESET = "could not set current environment",
    EEILL = "illegal environment name",
    EESWT = "could not switch to environment",

    EIREQ = "task id required",
    EINON = "no such task id",
    EICUR = "no current task id set",
    EIPRV = "no previous task id set",
    EIPRS = "could not switch to previous task id",
    EIDEL = "could not delete task id",
    EIADD = "could not add new task id",
    EIEXT = "task id already exists",
    EISET = "could not set current task id",
    EIILL = "illegal task id",
    EISWT = "could not switch to task id",

    EESPRV = "could not switch to previous environment",
    EISPRV = "could not switch to previous task",

    EEUADD = "could not add unit environment",
    EEUDEL = "could not delete unit environment",

    EETDEL = "could not delete task directory",
    EETADD = "could not create task directory",
}

---Get error description.
---@param desc string
---@return string
function errmod.get(desc)
    desc = errno or desc
    return desc
end

---Set error.
---@param desc string
function errmod.set(desc)
    errno = desc
end

return errmod
