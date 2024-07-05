all: release
.PHONY: lua_fmt lua_lint lua_tests

all: release


# stylua is install by cargo (rust something)
lua_fmt:
	@echo "===> Formatting"
	stylua -f .configs/stylua.toml src
	@echo

lua_lint:
	@echo "===> Linting"
	luacheck src
	@echo

lua_tests:
	@echo "\n===> Tests (under development)"
	#@lua tests/unit/tests.lua
	@echo

release: lua_fmt lua_lint lua_tests
