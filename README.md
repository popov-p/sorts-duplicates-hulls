# nppame

## Сборка и запуск
```bash
cd nppame
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_SANITIZERS=On && make
```

```bash
perf record -F 99 -g -- ./sort-comparison-exec \
~/devel/nppame/Flamegraph/stackcollapse-perf.pl out.perf > out.folded \
~/devel/nppame/Flamegraph/flamegraph.pl out.folded > flamegraph.svg
```