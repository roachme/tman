# Tman
Tman's terminal task manager.


# User configuration
## Plugins
Tman has three plugins (for right now)
1. `struct` - Create files and directory for each task (using templete defined
              in configuration file `user.json`)
2. `git`    - Add support to work with git repositories. Create symlinks for
              each task, task branches, support to pull from remote git server,
              rebase (update) task branch against default branch.
3. `make`   - Run make commands defined in Makefile, like tests, build.

### Struct
Config example to create two directories `images` and `docs` in each task
contained in environment `personal`. Issue `tman sync -s` to create them.
```
{
    "personal": {
        "struct": {
            "dirs": [
                "images",
                "docs"
            ]
        }
    }
}

```

### Git
To create symlink in task directory to git repo you with add the below config
into user.json. Next time you create task in environment `personal` tman create
symlink to repo `tman` and branch based on task description. Or issue command
`tman sync -G TASKID` to create repo symlink in existing task.
```
{
    "personal": {
        "git": {
            "repos": [
                {
                    "name": "tman",
                    "branch": "master",
                    "link": "https://github.com/roachme/tman",
                    "path": ""
                }
            ]
        }
    }
}
```

# Technical stuff

## Inner structure
Tman consist of four parts: core, aux, builtin and plugin.
Basic structure:
Tman structure:
Main:
    1. TaskID
    2. TaskUnit
    3. Config
    4. Git
    5. Struct
Aux:
    1. Log
    2. Debug

