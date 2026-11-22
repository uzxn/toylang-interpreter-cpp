set $n inputNum
set $i 0
while less $i $n {
    arraySet $a $i inputNum
    set $i add $i 1
}

set $i 0
while less $i sub $n 1 {
    set $min $i
    set $j add $i 1
    while less $j $n {
        if less arrayGet $a $j arrayGet $a $min {
            set $min $j
        }
        set $j add $j 1
    }
    set $tmp arrayGet $a $i
    arraySet $a $i arrayGet $a $min
    arraySet $a $min $tmp
    set $i add $i 1
}

set $i 0
while less $i $n {
    print arrayGet $a $i
    printChar 32
    set $i add $i 1
}
printChar 10
