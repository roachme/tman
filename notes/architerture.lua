--[[
Think
    1. module setup.lua should be part of module core.lua
    2. module shell.lua should be part of module taskid.lua

Modules
core:
    - tman      - project interface
    - core      - low level data structures and functions
    - taskid    - cache task unit's id
    - taskenv   - access current envname
    - taskunit  - create new task unit
    - builtin   - access builtin commands
    - aux/      - core auxilary stuff

part:
    - where put non-system non-user part of the project?
    - like
    - config.lua
    - help.lua

plugins:
    - struct    - create task directories and files (default/builtin plugin)
    - git       - create task branches in repos
    - make      - make commands to run

builtins:
    - add
    - arch
    - del
    ...
    ...
    - ver
]]
