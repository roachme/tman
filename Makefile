MAKE=make

.PHONY: cli lib lualib

cli:
	$(MAKE) -C cli $(PROGRAM)
	@echo "create cli version"

lib:
	$(MAKE) -C lib $(LIBSTATIC)
	@echo "create library"

liblua:
	@echo "create library for Lua"

