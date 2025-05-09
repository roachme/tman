## [Unreleased]

### Added

- `cli/help`: Add notion about new option
- `lib/tman`: Add skeleton to check board name

### Changed

- `cli/help`: Update command `add` help message
- `scripts/updver`: Update some of the logic
- `lib/tman`: Move name checks to common function
- `scripts/updver`: Delete `v` from version in CHANGELOG.md

### Fixed

- `scripts/updver`: Tag from master branch
- `cli/show`: Fix option parser
- `cli`: Fix linter warnings
- `tman`: Fix linter warnings
- `cli/del`: Fix project name check


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
