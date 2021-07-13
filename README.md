# desbench
A benchmark application for libdes

# Build System
libdes/desbench uses the [Bazel](https://bazel.build/) build system. Bazel is [easy to install](https://docs.bazel.build/versions/master/install.html).

# Dependencies
The only system level dependency libdes/desbench requires is libnuma.
``` sh
sudo apt install libnuma-dev
```
Everything else is automatically compiled in by bazel.

# Build desbench
``` sh
bazel build -c opt --copt=-O3 --copt=-g --copt=-flto --linkopt=-flto ...
```

# Run desbench
Run the benchmark simulator with default settings.
``` sh
./bazel-bin/desbench config/benchmark.json
```

Run the benchmark simulator with full parallelism.
``` sh
./bazel-bin/desbench config/benchmark.json /simulator/core/executers=int=$(nproc)
```

Run a full benchmark suite. This might require some additional python3 packages. If so, just use pip (`pip3 install matplotlib numpy taskrun --user`). This sweep will take 13\*3\*nproc\*5 seconds (26 minutes for 8 thread machine). For high thread count machines, use a more granular step (-s) such as 4 which reduces the number of simulations by 4x. Make sure your machine is not in use by other applications.
``` sh
./scripts/sweep.py ./bazel-bin/desbench output -r 3 -e 5 -s 1
```
