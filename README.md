# desbench

A benchmark application for libdes

# Build System
libdes/desbench uses the [Bazel](https://bazel.build/) build system. Bazel is [easy to install](https://docs.bazel.build/versions/master/install.html).

# Dependencies
The only system level dependency libdes/desbench requires is libnuma:
``` sh
sudo apt install libnuma-dev
```
Everything else is automatically compiled in by bazel.

# Build desbench
``` sh
bazel build -c opt --copt=-O3 --copt=-g --copt=-flto --linkopt=-flto ...
```

# Run desbench
Run the benchmark simulator with default settings:
``` sh
./bazel-bin/desbench cfg/benchmark.json
```

Run the benchmark simulator with full parallelism:
``` sh
./bazel-bin/desbench cfg/benchmark.json /simulator/core/executers=int=$(nproc)
```

Run a full benchmark suite (requires some python3 packages):

This will take 13*3*nproc*5 seconds (26 minutes for 8 thread machine)
``` sh
./scripts/sweep.py ./bazel-bin/desbench output -r 3 -e 5 -s 1
```
