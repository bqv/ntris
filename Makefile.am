SUBDIRS = build

run: all
	@pushd build; \
	ntris; \
	popd;

debug: all
	@pushd build; \
	ntris --debug; \
	popd;

test:
	@pushd build; \
	ntris; \
	popd;

gdb:
	@pushd bin; \
	gdb ntris; \
	popd;	
