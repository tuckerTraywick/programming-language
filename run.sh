CFLAGS="-std=c99 -Wall -Wpedantic -Wextra -g3"
LIBRARIES=""
clear
echo "---- BUILDING ----"
gcc $CFLAGS -Iinclude -Isource $LIBRARIES source/* -o build/run && echo && echo "---- RUNNING ----" && build/run
