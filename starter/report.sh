echo
echo $1 $2

./sim -f traceprogs/tr-simpleloop.ref -m $1 -s 999999 -a $2
./sim -f traceprogs/tr-matmul.ref -m $1 -s 999999 -a $2
./sim -f traceprogs/tr-blocked.ref -m $1 -s 999999 -a $2
./sim -f traceprogs/tr-mine.ref -m $1 -s 999999 -a $2
