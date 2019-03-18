#!/bin/sh
# ./ringmaster <port_num> <num_players> <num_hops>
make

echo "------------------------------------------------"
echo "------------------------------------------------"
echo "Ringmaster Tests"

echo "------------------------------------------------"
echo "Test 0:\tVALID\n./ringmaster 4444 10 10"
echo "------------------------------------------------"
./ringmaster 4444 10 10

echo "------------------------------------------------"
echo "Test 1:\tVALID\n./ringmaster 4444 1 10"
echo "------------------------------------------------"
./ringmaster 4444 1 10

echo "------------------------------------------------"
echo "Test 2:\tINVALID\n./ringmaster qrty wertyu dfgh rty"
echo "------------------------------------------------"
./ringmaster qrty wertyu dfgh rty # 4+1

echo "------------------------------------------------"
echo "Test 3:\tINVALID\n./ringmaster 122 1322"
echo "------------------------------------------------"
./ringmaster 122 1322 # 4-1

echo "------------------------------------------------"
echo "Test 4:\tINVALID\n./ringmaster qwe qwe qew"
echo "------------------------------------------------"
./ringmaster qwe qwe qew # correct # -- incorrect data format

echo "------------------------------------------------"
echo "Test 5:\tINVALID\n./ringmaster -123 -124 -132"
echo "------------------------------------------------"
./ringmaster -123 -124 -132 # -ve numbers

echo "------------------------------------------------"
echo "Test 6:\tINVALID\n./ringmaster    -123   -124 -132"
echo "------------------------------------------------"
./ringmaster    -123   -124 -132 # -ve numbers

echo "------------------------------------------------"
echo "------------------------------------------------"
echo "Player Tests"

echo "------------------------------------------------"
echo "Test 0:\tVALID\n./player 127.0.0.1 4444"
echo "------------------------------------------------"
./player 127.0.0.1 4444

echo "------------------------------------------------"
echo "Test 1:\tVALID\n./player vcm-6252.vm.duke.edu 4444"
echo "------------------------------------------------"
./player vcm-6252.vm.duke.edu 4444

echo "------------------------------------------------"
echo "Test 2:\tVALID\n./player localhost 4444"
echo "------------------------------------------------"
./player localhost 4444

echo "------------------------------------------------"
echo "Test 3:\tINVALID\n./player qrty wertyu dfgh"
echo "------------------------------------------------"
./player qrty wertyu dfgh # 3+1

echo "------------------------------------------------"
echo "Test 4:\tINVALID\n./player 122"
echo "------------------------------------------------"
./player 122 # 3-1

echo "------------------------------------------------"
echo "Test 5:\tINVALID\n./player qwe qwe"
echo "------------------------------------------------"
./player qwe qwe # correct # -- incorrect data format

echo "------------------------------------------------"
echo "Test 6:\tINVALID\n./player -123 -124"
echo "------------------------------------------------"
./player -123 -124 # -ve numbers

echo "------------------------------------------------"
echo "Test 7:\tINVALID\n./player    -123   -124"
echo "------------------------------------------------"
./player    -123   -124 # -ve numbers

echo "------------------------------------------------"
make clean
