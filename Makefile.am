SUBDIRS = build

run: all
	@pushd build; \
	4dtris; \
	popd;

debug: all
	@pushd build; \
	4dtris --debug; \
	popd;

test:
	@pushd build; \
	4dtris; \
	popd;

gdb:
	@pushd bin; \
	gdb 4dtris; \
	popd;	
