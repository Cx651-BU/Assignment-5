binary="test cli"

make clean; make

score=0

for file in $binary; do
    if [[ ! -f "$file" ]]; then
        echo "FAIL: $file not made"
        echo "SCORE: $score/7"
        exit 0
    fi
done


for (( i = 0; i < 4; i++ )); do
    echo "TEST: ./test $i"
    ./test $i > /dev/null
    if [ "$?" -eq 1 ]; then
    ((score+=1))
    else
    echo "  --FAIL!"
    fi
done


echo "TEST: valgrind ./cli kernel images/sky.bmp 640 426 out.bmp"
valgrind --leak-check=full --error-exitcode=1 ./cli kernel images/sky.bmp 640 426 out.bmp > /dev/null 2> valgrind.log

if [ "$?" -eq 0 ]; then
    ((score+=1))
else
    echo "  --FAIL!"
    echo "  Valgrind found memory errors:"
    cat valgrind.log
fi

echo "TEST: valgrind ./cli convert images/sky.bmp 640 426 out.bin"
valgrind --leak-check=full --error-exitcode=1 ./cli convert images/sky.bmp 640 426 out.bin > /dev/null 2> valgrind.log

if [ "$?" -eq 0 ]; then
    ((score+=1))
else
    echo "  --FAIL!"
    echo "  Valgrind found memory errors:"
    cat valgrind.log
fi

echo "TEST: valgrind ./cli mmap out.bin 640 426 out.bmp"
valgrind --leak-check=full --error-exitcode=1 ./cli mmap out.bin 640 426 out.bmp > /dev/null 2> valgrind.log

if [ "$?" -eq 0 ]; then
    ((score+=1))
else
    echo "  --FAIL!"
    echo "  Valgrind found memory errors:"
    cat valgrind.log
fi

echo "TEST: plotted images are present"
if [ -f "plot.png" ]; then
    ((score+=1))
else
    echo "  --FAIL!"
fi


echo "TEST: cli fault mode generates a pagefault"
output=$( /usr/bin/time -f "%F" ./cli fault x x x x 2>&1 )
faults=$(echo "$output" | tr -d '[:space:]')
if [ "$faults" -gt "0" ]; then
    ((score+=1))
else
    echo "  --FAIL!"
fi

echo "TEST: questions.txt is non-empty and contains questions (1) through (N)"
N=3
if [ -f questions.txt ] && [ "$(wc -c < questions.txt)" -gt 200 ]; then
    passed=true
    for ((i=1; i<=N; i++)); do
        if ! grep -q "^($i)" questions.txt; then
            echo "  --FAIL: questions.txt missing ($i)"
            passed=false
        fi
    done
    if $passed; then
        ((score+=1))
    fi
else
    echo "  --FAIL!"
fi

echo "SCORE: $score/10"
