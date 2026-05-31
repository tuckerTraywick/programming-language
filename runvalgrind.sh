./run.sh
clear
valgrind -s --leak-check=full --track-origins=yes build/test
