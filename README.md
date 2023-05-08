# Building

Clone repo with all its submodules:

```bash
git clone --recurse-submodules https://github.com/yca/valsim.git
```

Or if you already cloned it, update submodules:

```bash
git submodule update --init --recursive
```

Then create a build directory and build the project:

```bash
mkdir build && cd build
cmake ..
make -j8
```
