#!/bin/sh
# ./ringmaster <port_num> <num_players> <num_hops>
make

echo "------------------------------------------------"
echo "Test 0\n./ringmaster 4444 10 10"
echo "------------------------------------------------"
./ringmaster 4444 10 10

echo "------------------------------------------------"
echo "Test 1\n./ringmaster 4444 1 10"
echo "------------------------------------------------"
./ringmaster 4444 1 10

echo "------------------------------------------------"
echo "Test 2\n./ringmaster qrty wertyu dfgh rty"
echo "------------------------------------------------"
./ringmaster qrty wertyu dfgh rty # 4+1

echo "------------------------------------------------"
echo "Test 3\n./ringmaster 122 1322"
echo "------------------------------------------------"
./ringmaster 122 1322 # 4-1

echo "------------------------------------------------"
echo "Test 4\n./ringmaster qwe qwe qew"
echo "------------------------------------------------"
./ringmaster qwe qwe qew # correct # -- incorrect data format

echo "------------------------------------------------"
echo "Test 5\n./ringmaster -123 -124 -132"
echo "------------------------------------------------"
./ringmaster -123 -124 -132 # -ve numbers

echo "------------------------------------------------"
echo "Test 6\n./ringmaster    -123   -124 -132"
echo "------------------------------------------------"
./ringmaster    -123   -124 -132 # -ve numbers

echo "------------------------------------------------"
make clean
