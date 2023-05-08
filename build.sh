#!/bin/bash

print_section()
{
	echo "************************************ $1 ***********************************"
}

# exit when any command fails
set -e

# keep track of the last executed command
trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG
# echo an error message before exiting
trap 'echo "\"${last_command}\" command filed with exit code $?."' EXIT

#update submodules
git submodule update --init --recursive

#start from a clean state
rm -rf build bpf_tools/libbpf-bootstrap/build commonpp/build bpf_tools/build bpf_tools/rpclib/build bpf_tools/build

print_section "Building commonpp"
#build commonpp
pushd commonpp
mkdir -p build
cd build
cmake ..
make -j8
popd

#build bpf_tools
pushd bpf_tools

print_section "Building BPF examples"
mkdir -p libbpf-bootstrap/build
pushd libbpf-bootstrap/build
cmake ../examples/c
make
popd

#build rpclib
print_section "Building rpclib"
pushd rpclib
git apply ../rpclib_fpic.patch
mkdir -p build
cd build
cmake ..
make -j8
popd

#build preload
print_section "Building preload"
pushd ld_preload
make
popd

print_section "Building bpf tools itself"
mkdir -p build
cd build
cmake ..
make -j8

popd #bpf_tools

print_section "Building valsim"
mkdir -p build
cd build
cmake ..
make -j8

