## Tman
##### Terminal task manager

![command example](docs/tman.gif)


## ⇁  Problems
During my work I encounter many subtaks I gotta do on the daily basis.
Among them
1. Clone Git repos to my machine and create branches in multiple repos (usually different for each task)
2. Keep my task logs, sketches, notes and repos for each task somewhere. Structured and seperately
3. Often switch between task, which involves context, notes, sketches, and especially Git branches

## ⇁  Solution
Tman comes with 3 basic ideas
1. Structure your workspace for each task
2. Get you exactly where you want with fewest keystrokes
3. Automate stuff that you're tired of during the work. Here plugins are for

## ⇁  Structure
- Customize: Adjust the util for your workflow via config file.
- Extensible: Extend util with predefined or your own plugins.  You can get full list of Tman plugins in `tman-pgn`.

## ⇁  Dependencies
No external dependencies, the whole program's written in C. Tho plugins might have dependencies. Take a look at plugins README.md for more info.

## ⇁  Build
Simply run the command below.
```bash
make
```

## ⇁  Installation
1. Once you compiled successfully, put executable `_tmancli` into one of directories defined in env variable `PATH`. I put it in `~/.locil/bin` .
2. Copy the content of tman.sh into your shell rc file. It's `~/.bashrc`, `.zshrc`, etc.

```bash
#!/usr/bin/env sh

function tman()
{
    output="$(_tmancli "$@")"
    retstatus="$?"
    if [ "$retstatus" -eq 0 ]; then
        cd "$output"
    else
        [ -n "$output" ] && echo "$output"
    fi

    # Status 0 - ok, no error, no need to explain anything.
    # Status 1 - ok, but output is not a path, so juts output it.
    [ "$retstatus" -eq 1 ] && return 0 || return "$retstatus"
    return "$retstatus"
}
```
3. Create basic Tman config file. Either in `~/.tman/tman.cfg` or `~/.config/tman/tman.cfg` and fill it with content below

```
# Necessary variables
# Where to crease base for your future tasks
TMANBASE = /home/user/tman
# Where your plugins are installed
TMANPGNINS = /home/user/.tman/pgnins

# Set of hooks, uncomment them IF you really want them and you have defined config files for them
#HOOKCMD = add struct create
#HOOKCMD = add gun sync
#HOOKCMD = sync gun sync
#HOOKCMD = prev gun sync
#HOOKCMD = del gun del
#HOOKCMD = prev gun sync
#HOOKCAT = cat time cat
#HOOKCAT = cat gun cat
#HOOKLIST = list tag list
```

## ⇁  Tman builtin commands
```
Usage: tman [OBJECT] COMMAND [OPITONS]... [ID]...
    Where OBJECT := { id | prj | pgm }
    Where COMMAND is one of the builtin commands
    Where ID is task ID
Try 'tman help COMMAND' for more info.

System:
  cfg    - Manage system configuration file.
  chk    - Check and repair a system.
  help   - Show help message for command.
  init   - Init directory structure.

Basic:
  add    - Add a new task to project.
  del    - Delete task from project.
  list   - List project tasks.
  prev   - Switch to previous task in current project.
  show   - Show task info.
  sync   - Switch to or synchronize (with) task(s).

Misc:
  find   - Find a task by pattern.
  flow   - Show and set project workflow.
  grep   - Find pattern in a task.
  col    - Move task to column.
  link   - Link tasks together.
  move   - Move (rename) tasks.
  set    - Set task unit values.

Object:
  prj    - Manage and show projects.
  pgm    - Manage and show plugins.
```

## ⇁  Basic workflow
1. To initalize util type in `tman init`.
2. Now you're ready to create a project: `tman prj add test`
3. Once task project is created you can fill it with tasks: `tman add test1`
4. List all your task in current project: `tman list`
5. Show the content of task: `tman cat test1`


## ⇁  Tips
1. Use `tman help` to get list off commands.
2. Or `tman help COMMAND` to get help on specified command.
