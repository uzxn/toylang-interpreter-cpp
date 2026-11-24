set $n in

set $i 1
while not gre $i $n {
    aset $a $i in
    set $i add $i 1
}

func $swap [ $i $j ] {
    set $t aget $a $i
    aset $a $i aget $a $j
    aset $a $j $t
}

func $merge_sort [ $begin $end ] {
    if not les $begin $end {
        return 0
    }

    set $mid div add $begin $end 2
    
    call $merge_sort [ $begin $mid ]
    call $merge_sort [ add $mid 1 $end ]

    set $i $begin
    set $left $begin
    set $right add $mid 1

    while and not gre $left $mid not gre $right $end {
        if not gre aget $a $left aget $a $right {
            aset $b $i aget $a $left
            set $left add $left 1
            set $i add $i 1
        } else {
            aset $b $i aget $a $right
            set $right add $right 1
            set $i add $i 1
        }
    }

    while not gre $left $mid {
        aset $b $i aget $a $left
        set $left add $left 1
        set $i add $i 1
    }

    while not gre $right $end {
        aset $b $i aget $a $right
        set $right add $right 1
        set $i add $i 1
    }

    set $i $begin
    while not gre $i $end {
        aset $a $i aget $b $i
        set $i add $i 1
    }
}

func $quick_sort [ $begin $end ] {
    if not les $begin $end {
        return 0
    }

    set $pivot aget $a $begin
    set $p1 set $p2 sub $begin 1

    set $i $begin
    while not gre $i $end {
        if gre $pivot aget $a $i {
            call $swap [ $i add $p2 1 ]
            call $swap [ add $p2 1 add $p1 1 ]
            set $p1 add $p1 1
            set $p2 add $p2 1
        } else if eq aget $a $i $pivot {
            call $swap [ $i add $p2 1 ]
            set $p2 add $p2 1
        }
        set $i add $i 1
    }

    call $quick_sort [ $begin $p1 ]
    call $quick_sort [ add $p2 1 $end ]
}

call $merge_sort [ 1 $n ]

set $i 1
while not gre $i $n {
    out aget $a $i
    cout 32
    set $i add $i 1
}

cout 10
