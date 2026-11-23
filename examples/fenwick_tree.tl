set $n in

set $i 0
while not gre $i $n {
    aset $bit $i 0
    set $i add $i 1
}

set $i 1
while not gre $i $n {
    set $delta in
    set $j $i
    while not gre $j $n {
        aset $bit $j add aget $bit $j $delta
        set $j add $j & $j sub 0 $j
    }
    set $i add $i 1
}

set $q in
set $k 0
while les $k $q {
    set $op in
    set $idx in
    if eq $op 1 {
        set $delta in
        set $j $idx
        while not gre $j $n {
            aset $bit $j add aget $bit $j $delta
            set $j add $j & $j sub 0 $j
        }
    }
    if eq $op 2 {
        set $sum 0
        set $j $idx
        while gre $j 0 {
            set $sum add $sum aget $bit $j
            set $j sub $j & $j sub 0 $j
        }
        out $sum
        cout 32
    }
    set $k add $k 1
}
cout 10
