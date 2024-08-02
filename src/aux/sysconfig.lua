--- Responsible for system config.
-- @module sysconfig

local sysconfig = {}
local configcontent
local fconfig
local config_vars = { "prefix", "install" }

local function check_config_var(var)
    for _, cvar in pairs(config_vars) do
        if var == cvar then
            return true
        end
    end
    return false
end

local function load_sysconfig()
    configcontent = {}
    local f = io.open(fconfig)

    if not f then
        io.stderr:write("couldn't open system config file to read\n")
        os.exit(1)
    end

    for line in f:lines() do
        -- skip commetns
        if not string.match(line, "#") then
            local key, val = string.match(line, "(%w*)%s*=%s*(.*)")
            if not check_config_var(key) then
                io.stderr:write(
                    "load: error: no such key: ",
                    key or "no key",
                    "\n"
                )
                os.exit(1)
            end
            configcontent[key] = val
        end
    end
end

local function save_sysconfig()
    local warning = "# NOT recommended to change this file manually.\n"
    local f = io.open(fconfig, "w")

    if not f then
        io.stderr:write("couldn't open system config file to write\n")
        os.exit(1)
    end

    f:write(warning)
    for key, val in pairs(configcontent) do
        f:write(("%s = %s\n"):format(key, val))
    end
    return f:close() == true
end

function sysconfig.set(key, val)
    if not check_config_var(key) then
        io.stderr:write("sysconfig: unknown config var: ", key or "no var")
        os.exit(1)
    end
    configcontent[key] = val
    save_sysconfig()
end

function sysconfig.get(var)
    return configcontent[var]
end

function sysconfig.show()
    for key, value in pairs(configcontent) do
        print(("%s = %s"):format(key, value))
    end
end

function sysconfig.init(fname)
    fconfig = fname
    load_sysconfig()
end

function sysconfig.getvars()
    return {
        prefix = sysconfig.get("prefix"),
        install = sysconfig.get("install"),
    }
end

return sysconfig
