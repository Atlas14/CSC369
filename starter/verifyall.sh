make
./sim -f benchmark/tr-simpleloop.ref -m 200 -s 99999 -a rand
./sim -f benchmark/tr-simpleloop.ref -m 200 -s 99999 -a fifo
./sim -f benchmark/tr-simpleloop.ref -m 200 -s 99999 -a lru
./sim -f benchmark/tr-simpleloop.ref -m 200 -s 99999 -a clock
echo
echo "---"
./sim -f benchmark/tr-simpleloop.ref -m 200 -s 99999 -a rand -v benchmark/tr-simpleloop.versions
./sim -f benchmark/tr-simpleloop.ref -m 200 -s 99999 -a fifo -v benchmark/tr-simpleloop.versions
./sim -f benchmark/tr-simpleloop.ref -m 200 -s 99999 -a lru -v benchmark/tr-simpleloop.versions
./sim -f benchmark/tr-simpleloop.ref -m 200 -s 99999 -a clock -v benchmark/tr-simpleloop.versions
echo
echo "opt:"
./sim -f benchmark/tr-simpleloop.ref -m 200 -s 99999 -a opt
