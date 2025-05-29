
cub:
	nvcc task8_v2.cu -o task8 -O3 -std=c++17 -lboost_program_options -lcudart
	./task8

profile:
	nsys profile --trace=nvtx,cuda --stats=true ./task8 --max_iterations=100