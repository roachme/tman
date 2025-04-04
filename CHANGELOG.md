## [Unreleased] - (format: YYYY-MM-DD)

### Added

### Changed

- `cli`: Change API option `-b' to `-B'

### Fixed

- `cli/add`: Fix task ID length check

### Deleted

### Deprecated


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
