all: build

mkdir_build:
	rm -rf build
	mkdir -p build

build: mkdir_build
	cd build; cmake ..
	make -C build

debug: mkdir_build
	cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..
	make -C build

run:
	cd build; ./burningDude

