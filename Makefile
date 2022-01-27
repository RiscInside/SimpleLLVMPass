CLANG=clang-11
OPT=opt-11

test: build
	$(CLANG) -c -emit-llvm program.c
	$(OPT) -load build/libCountPass.so -count < program.bc > /dev/null

build: build/libCountPass.so

build/libCountPass.so:
	mkdir -p build
	cmake -G "Unix Makefiles" -B build
	make -C build
