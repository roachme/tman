<div align="center">
   <!--<img align="center" width="128px" src="crates/gitbutler-tauri/icons/128x128@2x.png" /> -->
	<h1 align="center"><b>Tec</b></h1>
	<p align="center">
        Terminal project and task  manager
    <br />
  </p>
</div>

<br/>


![command example](docs/tec.gif)


## ⇁  Problems
During my work I encounter many subtaks I gotta do on the daily basis.
Among them
1. Clone Git repos to my machine and create branches in multiple repos (usually different for each task)
2. Keep my task logs, sketches, notes and repos for each task somewhere. Structured and seperately
3. Often switch between task, which involves context, notes, sketches, and especially Git branches

## ⇁  Solution
Tec comes with 3 basic ideas
1. Structure your workspace for each task
2. Get you exactly where you want with fewest keystrokes
3. Automate stuff that you're tired of during the work. Here plugins are for

## ⇁  Structure
- Customize: Adjust the util for your workflow via config file.
- Extensible: Extend util with predefined or your own plugins. You can get full list of Tec plugins in ``` tec-pgn ```.

## ⇁  Dependencies
Plugins might have dependencies. Take a look at plugins README.md for more info.
Main program uses the some dependencies. To install them run commands below:
```
sudo apt install -y libconfig-dev
```


## ⇁  Build
Simply run the command below.
``` bash
make
```

## ⇁  Installation
1. Once you compiled successfully, put executable ` teccli ` into one of directories defined in env variable ` PATH ` . I put it in ` ~/.local/bin ` .
2. Copy the content of tec.sh into your shell rc file. It's ` ~/.bashrc ` , ` .zshrc ` , etc.

``` bash
#!/usr/bin/env bash

function tec()
{
    local tecstatus;
    local pwdfile="/tmp/tecpwd"

    teccli "$@"
    tecstatus="$?"

    test -s "$pwdfile" && cd "$(cat "$pwdfile")" || return "$tecstatus"
    return "$tecstatus"
}
```

3. Create basic Tec config file. Either in ` ~/.tec/tec.cfg ` or ` ~/.config/tec/tec.cfg ` and fill it with content below

```
base = {
    task = "$HOME/tectask";
    pgn = "$HOME/.local/lib/tec/pgn";
};

options = {
    hook = true;    /* enable hooks */
    color = true;   /* enable colors */
    debug = false;  /* disable debug info */
};

/* list of hooks for all projects */
hooks = {
    show = (
        { bincmd = "show"; pgname = "gmux"; pgncmd = "show" },
    );
    action = (
        { bincmd = "add"; pgname = "gmux"; pgncmd = "sync" },
    );
    list = ();
};
```

## ⇁  Tec builtin commands
```
Usage: tec [OPTION]... COMMAND|PLUGIN
    Run 'tec help tec' to get more info.

    System:
      help       - Show help for commands.
      init       - Init directory structure.
      version    - Show program version.

    Basic:
      add        - Add a new task to project.
      del        - Delete task from project.
      list       - List project tasks.
      move       - Move (rename) tasks (under development).
      prev       - Switch to previous task.
      set        - Set task unit values.
      show       - Show task unit values.
      sync       - Synchronize (with) task.

    Object:
      board      - Manage and show boards.
      column     - Manage and show columns.
      project    - Manage and show projects.

```


## ⇁  Plugins

There is a ` pgm ` plugin manager to install plugins.

Here's some plugin to check out \
` pgm ` - [Tec plugin manager](https://github.com/roachme/tec-pgm.git) \
` gmux ` - [Manage bunch of git repos](https://github.com/roachme/tec-gmux.git) \
` find ` - [Find stuff in tasks](https://github.com/roachme/tec-find.git)


## ⇁  Install plugin manager

To install plugin manager run the code below
```
export PGNDIR="$HOME/.local/lib/tec/pgn"
git clone https://github.com/roachme/tec-pgm.git "$PGNDIR/pgm"
```
Note: \
1. Make sure repo dirname has NO prefix ` tec- `. The same goes about any plugin to install \
2. ` "$PGNDIR" ` is the path set in ` tec.cfg `.
   Make sure ` PGNDIR ` is the same as ` base.pgn ` in ` tec.cfg ` shown above.


## ⇁  Basic workflow
1. To initalize util type in ` tec init `
2. Now you're ready to create a project: ` tec project add test `
3. Once task project is created you can fill it with tasks: ` tec add test1 `
4. List all your task in current project: ` tec list `
5. Show the content of task: ` tec show test1 `
6. Sync with current task: ` tec sync `
7. Or quickly switch to previous task: ` tec prev `


## ⇁  Tips
1. Use ` tec help ` to get list of commands.
2. Or ` tec help COMMAND ` to get help on specified command.
