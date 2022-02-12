#! /bin/bash                                                                    

sleep 1
for ((i=1; i<=3; i++))
do
./player 0.0.0.0 4444 & 
done

wait