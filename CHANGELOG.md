## [Unreleased]

### Added

- `cli/config`: Add default values
- `cli/config`: Use libconfig package for config

### Changed

- `cli/link`: revert "Add skeleton to link tasks"
- `pgn`: Move plugins to submodule

### Fixed

- `pgn`: Update option for task directory
- `cli/show`: Hotfix show hooks

### Deleted

- `cli`: Delete link command skeletons


## [0.0.15] - (2025-06-02)

### Added

- `pgn/find`: Add plugin to find in database
- `cli/prj`: Add command `set` to project object
- `cli/prj`: Add command `show` to project object
- `scripts/updver`: Add help message command

### Changed

- `tman`: Refactor unit module
- `lib/unit`: Update unit module's API

### Fixed

- `cli/help`: Update help message
- `cli/plugin`: Fix plugin options parser
- `cli/cli`: Fix config options
- `cli`: Fix missing usehooks option


## [0.0.14] - (2025-05-24)

### Added

- `cli/move`: Add option `-h` for help
- `cli/config`: Add support for environment variable `HOME` in path
- `scripts/install`: Add skeleton for installation script
- `cli/list`: Add option to show project name header

### Changed

- `cli/config`: Change depricated hook names

### Fixed

- `scripts`: Fix a typo in directory name
- `cli/set`: Fix options mismatching with help message


## [0.0.13] - (2025-05-15)

### Added

- `scripts/makevideo`: Add a script to produce video of how to use tman
- `cli/prj`: Add prompt before deleting a project
- `cli`: Pass system config values via option

### Changed

- `lib`: Delete depricated function
- `cli/help`: Update help messages
- `lib/tman`: Rename structure member
- `lib`: Add prefix `LIB` to library macros

### Fixed

- `scripts/updver`: Fix tag update
- `lib/tman`: Get new current arguments when switch to previous task
- `cli/prj`: Fix option `-h` for project subcommands

### Deleted

- `tman`: Delete unused header files


## [0.0.12] - (2025-05-10)

### Added

- `cli/help`: Add notion about new option
- `lib/tman`: Add skeleton to check board name
- `lib/task`: Add support to move current task to any column

### Changed

- `cli/help`: Update command `add` help message
- `scripts/updver`: Update some of the logic
- `lib/tman`: Move name checks to common function
- `scripts/updver`: Delete `v` from version in CHANGELOG.md
- `lib`: Rename confusing private function names

### Fixed

- `scripts/updver`: Tag from master branch
- `cli/show`: Fix option parser
- `cli`: Fix linter warnings
- `tman`: Fix linter warnings
- `cli/del`: Fix project name check
- `lib/task`: Fix move task to previous column if current not set
- `cli/show`: Fix invalid option specification

### Optimized

- `lib/tman`: Optimize switch to previous task


## [v0.0.11] - (2025-05-07)

### Added

- `cli/help`: Delete command grep from manual
- `cli`: Add ID generation to cli part
- `cli`: Add fixed padding to output
- `cli/help`: Add help message for util itself
- `scripts/updver`: Refactor logic
- `cli`: Move version to file

### Changed

- `cli/help`: Amend help message for command `prj`
- `tman`: Rename lib API names from id to task

### Fixed

- `lib`: Fix minor linter warnings
- `cli`: Move project name check outta loop

### Deleted

- `cli/help`: Delete option '-h'
- `cli/add`: Delete option for task ID generation from manual


## [0.0.10] - (2025-04-20)

### Added

- `cli/cli`: Add program name before error messages
- `Makefile`: Add release build
- `lib/col`: Show list of project columns
- `tman`: Add command to rename project

### Changed

- `Makefile`: Rename executable filename to `_tmancli`
- `README`: Update shell script
- `lib`: Make check function's return value boolean
- `README`: Update README.md
- `cli/help`: Amend help for prj-rename command

### Fixed

- `tman`: Fix linter warnings
- `pgn/time`: Fix invalid option
- `lib/tman`: Fix wrong error code

### Deleted

- `cli/cli`: Delete command ver (replace it with option)
- `lib/common`: Delete unused function declarations


## [0.0.9] - (2025-04-09)

### Changed

- `cli`: Change API option `-b` to `-B`
- `pgn`: Make pgn submodule part of codebase

### Fixed

- `cli/add`: Fix task ID length check


## [0.0.8] - (2025-03-31)

### Added

- `cli/show`: Add option `-k' to command show
- `cli/move`: Add support to move and rename tasks

### Changed

- `tman`: Use unified structure for options
- `tman`: Move hooks to cli part
- `tman`: Move config to cli part
- `tman`: Rename struct tman_args to struct tman_arg
- `lib`: Rename preprocessor directive check

### Fixed

- `cli`: Fix a memory leak in config module
- `cli`: Fix linter warnings
- `lib/tman`: Fix build warning about overflow

### Deleted

- `cli/cli`: Delete command find
- `lib/tman`: Delete task API functions getcurr() and getprev()
- `lib/tman`: Delete project API functions getcurr() and getprev()


## [0.0.7] - (2025-03-24)

### Added

- `scripts`: Refactor logic for script updver.sh
- `cli/find`: Add support for project name
- `tman`: Add linux kernel style (trade spaces for tabs)

### Changed

- `cli/list`: Show plugin braces only there's plugin output
- `lib/tman`: Change return value of function tman_deinit()
- `lib/unit`: Change return value of function unit_delpgn()
- `lib/tree`: Change return value of function tree_free()
- `tman`: Use unified structure for input arguments

### Fixed

### Deleted

- `lib`: Remove variable PROGRAM from lib part
- `lib`: Remove logging functions
- `lib/tman`: Delete library functions' return code
- `lib/tman`: Delete fprintf() from library calls
- `cli/cli`: Delete command list as default


## [0.0.6] - (2025-03-14)

### Added

- `Makefile`: Turn on compiler warning messages
- `docs`: Add a usage video


## [0.0.5] - (2025-03-03)

### Added

- `CHANGELOG`: Add a structure to CHANGELOG logs
- `tman`: Add support to not switch to newly created project
- `cli`: Add extra space between task/project tag and description
- `lib/dir`: Add support to create project hidden directory
- `cli/prev`: Add option quiet to suppress error messages
- `cli/list`: Add filter options for task list

### Changed

- `cli/prj`: Delete extra space between columns
