clang++ -o runner runner.cpp -g
# ON WINDOWS YOU WOULD USE -shaded INSTEAD OF -dynamiclib
# you would also want to call the lib game.dll, to follow conventions.
clang++ -o libgame.so game.cpp -dynamiclib -g
