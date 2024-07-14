local env = require("core.env")
local core = require("core.core")
local git = require("secondary.git")
local config = require("secondary.config")
local utils = require("aux.utils")

local function tman_init()
    local uconfig
    local envname = env.getcurr()
    local path = core.struct.code.path

    if not core.create() then
        core.die(1, "could not create core structure", "init")
    end

    -- roachme: guess it's better to leave it to command env.
    -- download repos when create new environment.
    uconfig = config.uget(envname)
    for _, repo in pairs(uconfig.repos) do
        if not utils.access(path .. "/" .. repo.name) then
            if not git.repo_clone(repo.link, repo.name, path) then
                print("name", repo.name, path)
                core.die(1, "could not download repo '%s'", "init", repo.name)
            end
        end
    end
    return 0
end

return tman_init
