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
2. Result

Optimized method:
* Transpose B matrix to make it read as column major, for matrix A * B, to improve spatial locality
* Apply blocking (tiling) to improve cache hit rates

OMP versus Optimized:

<img width="468" alt="image" src="https://github.com/123standup/Cache-Coherence/assets/59056739/151a309e-14dd-48b2-8ccf-07a2d0f4617a">



## coherence_protocol_simulator
