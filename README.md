# Cache-Coherence
## GEMM_optimization
1. Setup
```
# compile code
make serial
# generate test data
make gen_matrix
# run
make run_serial, run_omp
```
2. Result \
Optimized method:
* Transpose B matrix to make it read as column major, for matrix A * B, to improve spatial locality
* Apply blocking (tiling) to improve cache hit rates

OMP versus Optimized: \
<img width="468" alt="image" src="https://github.com/123standup/Cache-Coherence/assets/59056739/151a309e-14dd-48b2-8ccf-07a2d0f4617a">



## coherence_protocol_simulator
In this project, a simulator is created to compare different coherence protocol optimizations. The simulator performs on a 4-processor system with a Modified MSI and Dragon Protocol. The number of cycles and data transfer are not under consideration.

1. Cache parameters \
Size: 8192B, associativity: 8, block size: 64B

2. Setup
```
# compile code
make
# run
make run
```

3. Used Protocols
* Modified MSI (A simplified version of the original MSI) \
![image](https://github.com/123standup/Cache-Coherence/assets/59056739/121627cc-89a0-4ba8-a6d4-655052efb5e4)
* Dragon Protocol \
![image](https://github.com/123standup/Cache-Coherence/assets/59056739/5fa8e2f7-94b6-46ae-9e45-5819d20e4046)
