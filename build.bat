g++ -pthread -march=x86-64 -Og -s -std=c++11 -Wall -Wextra -Wshadow -I"./src/include" "./src/rng.cpp" "./src/color_palette.cpp" "./src/main.cpp" -o "./buddhabrot.exe"