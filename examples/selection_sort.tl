set $n in
set $i 0
while les $i $n {
    aset $a $i in
    set $i add $i 1
}

set $i 0
while les $i sub $n 1 {
    set $min $i
    set $j add $i 1
    while les $j $n {
        if les aget $a $j aget $a $min {
            set $min $j
        }
        set $j add $j 1
    }
    set $tmp aget $a $i
    aset $a $i aget $a $min
    aset $a $min $tmp
    set $i add $i 1
}

set $i 0
while les $i $n {
    out aget $a $i
    cout 32
    set $i add $i 1
}
cout 10
