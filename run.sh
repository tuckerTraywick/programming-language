tput reset
cmake -Bbuild -S. && cmake --build build && echo "" && ./build/run
