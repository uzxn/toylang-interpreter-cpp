set $n inputNum

set $i 0
while not greater $i $n {
    arraySet $bit $i 0
    set $i add $i 1
}

set $i 1
while not greater $i $n {
    set $delta inputNum
    set $j $i
    while not greater $j $n {
        arraySet $bit $j add arrayGet $bit $j $delta
        set $j add $j bitAnd $j sub 0 $j
    }
    set $i add $i 1
}

set $q inputNum
set $k 0
while less $k $q {
    set $op inputNum
    set $idx inputNum
    if equal $op 1 {
        set $delta inputNum
        set $j $idx
        while not greater $j $n {
            arraySet $bit $j add arrayGet $bit $j $delta
            set $j add $j bitAnd $j sub 0 $j
        }
    }
    if equal $op 2 {
        set $sum 0
        set $j $idx
        while greater $j 0 {
            set $sum add $sum arrayGet $bit $j
            set $j sub $j bitAnd $j sub 0 $j
        }
        print $sum
        printChar 32
    }
    set $k add $k 1
}
printChar 10
