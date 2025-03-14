# Сборака CMake

# Сборка с Float
```
dir build
cd build
cmake ..  
cmake --build . 
```

# Сборка с Double
```
dir build
cd build
cmake -DUSE_DOUBLE=ON ..  
cmake --build .
```

Float:  
```
Array type: f  
Sum of array elements: -0.0277862  
Time: 0.342616 seconds  
```

Double:  
```
Array type: d  
Sum of array elements: 4.89582e-11  
Time: 0.342837 seconds  
```
