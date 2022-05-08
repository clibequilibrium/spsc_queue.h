# Lock-free Unbounded SPSC Queue

Unbounded single-producer/single-consumer node-based queue. Dequeue operations are wait-free, enqueue operations are wait-free (in most cases) when there is an available node in cache; otherwise a new node will be allocated via malloc.

## Credits
[Dmitry Vyukov](https://www.1024cores.net/home/lock-free-algorithms/queues/unbounded-spsc-queue) C++ implementation

[Steffen Vogel](https://github.com/stv0g/c11-queues) C implementation and benchmarking

[An Efficient Unbounded Lock-Free Queue
for Multi-core Systems] (https://link.springer.com/content/pdf/10.1007/978-3-642-32820-6_65.pdf)

## License
[MIT license](https://choosealicense.com/licenses/mit/)