SUBDIRS = build

run: style all
	@pushd build; \
	ntris; \
	popd;

dbg: all
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

style:
	astyle -z2 -c -A1 -s4 -q -n -r '*.c' '*.h'
