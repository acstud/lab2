module use /opt/insy/modulefiles
module load cuda
module load devtoolset/8

mkdir debug
cd debug
cmake3 -DCMAKE_CUDA_FLAGS="-arch=compute_50" ..
make -j4
./imgproc-benchmark -a ../images/42.png
