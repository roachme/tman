local core = require("core.core")
local git = require("secondary.git")
local config = require("secondary.config")

local function tman_init()
    local path = core.struct.code.path

    if not core.create() then
        core.die(1, "could not create core structure", "init")
    end

    for _, repo in pairs(config.user.repos) do
        if not git.repo_clone(repo.link, repo.name, path) then
            core.die(1, "could not download repo '%s'", "init", repo.name)
        end
    end
    return 0
end

return tman_init
