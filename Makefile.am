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

style:
	astyle -z2 -c -A1 -s4 -n -r '*.c' '*.h'
