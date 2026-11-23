set $n in

set $i 0
while les $i $n {
    aset $a $i in
    set $i add $i 1
}

set $sp 0
if gre $n 1 {
    aset $lo 0 0
    aset $hi 0 sub $n 1
    set $sp 1
}

while gre $sp 0 {
    set $sp sub $sp 1
    set $l aget $lo $sp
    set $r aget $hi $sp
    set $p aget $a $r
    set $i sub $l 1
    set $j $l
    while les $j $r {
        if les aget $a $j $p {
            set $i add $i 1
            set $tmp aget $a $i
            aset $a $i aget $a $j
            aset $a $j $tmp
        }
        set $j add $j 1
    }
    set $i add $i 1
    set $tmp aget $a $i
    aset $a $i aget $a $r
    aset $a $r $tmp
    if les $l sub $i 1 {
        aset $lo $sp $l
        aset $hi $sp sub $i 1
        set $sp add $sp 1
    }
    if les add $i 1 $r {
        aset $lo $sp add $i 1
        aset $hi $sp $r
        set $sp add $sp 1
    }
}

set $i 0
while les $i $n {
    out aget $a $i
    cout 32
    set $i add $i 1
}
cout 10
