cpu_single:
	pgc++ -o task6_cpu -lboost_program_options -acc=host -Minfo=all -I/opt/nvidia/hpc_sdk/Linux_x86_64/23.11/cuda/12.3/include task6_cpu.cpp
	./task6_cpu --size=256 --accuracy=0.000001 --max_iterations=1000000

cpu_multi:
	pgc++ -o task6_multicore -lboost_program_options -acc=multicore -Minfo=all -I/opt/nvidia/hpc_sdk/Linux_x86_64/23.11/cuda/12.3/include task6_cpu.cpp
	./task6_multicore --size=256 --accuracy=0.000001 --max_iterations=1000000

gpu:
	pgc++ -o task6_gpu -lboost_program_options -acc=gpu -Minfo=all -I/opt/nvidia/hpc_sdk/Linux_x86_64/23.11/cuda/12.3/include task6_gpu.cpp
	./task6_gpu --size=1024 --accuracy=0.000001 --max_iterations=1000000
profile:
	nsys profile --trace=nvtx,cuda,openacc --stats=true ./task6_gpu --size=512 --accuracy=0.0001 --max_iterations=100