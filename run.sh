CC="gcc"
CFLAGS="-I. -g3 -Wall -Wextra -Wpedantic"
clear
echo "-------- BUILDING --------"
rm -rf build
mkdir -p build
cd build
"${CC}" ../source/*.c -o run "${CFLAGS}" && echo && echo "-------- RUNNING --------" && ./run
cd ..
