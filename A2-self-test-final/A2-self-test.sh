#!/bin/bash

GOLDENDIR=/Users/Ray/CSC369/A2-self-test-final
USER=`whoami`


cd /Users/Ray/CSC369/starter


# Run make
echo "Running make..."
echo ""

make




cp $GOLDENDIR/traces/* .

MAX=2
MAXTOTAL=10

TOTAL=0
for algo in rand opt fifo lru clock; do
	MARK=0
	for trace in 1_trace 4_trace; do
		./sim -f $trace -m 8 -s 12 -a $algo > $trace.out
		diff -b $trace.out $GOLDENDIR/$algo/$trace.golden.out > /dev/null

		if [ $? -eq 0 ]; then
			MARK=$(($MARK+1))
		fi
	done
		
	if [ $algo = "rand" ]; then
		echo "Mark for pagetable implementation: $MARK/$MAX"
	else
		echo "Mark for $algo: $MARK/$MAX"
	fi
	TOTAL=$(($TOTAL+$MARK))
done

echo ""

echo "Total Mark: $TOTAL/$MAXTOTAL"

rm -rf *trace*

