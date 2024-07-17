Tool to utilize workflow in embedded development

Status: Active development

## How to install

# Default places to search config file

# Default config structure


## How to use

# Geneal help

# More descriptive help message


## Inner structure
Tman consist of four parts: core, aux, builtin and plugin.


# Plugin
Tman has three plugins (for right now)
1. `struct` - Create files and directory for each task (using templete defined
              in configuration file `user.json`)
2. `git`    - Add support to work with git repositories. Create symlinks for
              each task, task branches, support to pull from remote git server,
              rebase (update) task branch against default branch.
3. `make`   - Run make commands defined in Makefile, like tests, build.


Config repo:
File `repos` contain list of repos that might be possibly used at work. The file
structure is shown below. Values are set in CSV.

```
repo_name,default_branch,repo_path

Example:
myutil,master,
```

repo_name - name of the repo
default_branch - branch to branch from and merge back to.
repo_path - if repo's nested specify its path here, otherwise leave it emtpy,
            default will be used.


## Code structure

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

