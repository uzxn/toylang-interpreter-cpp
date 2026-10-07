set $n inputNum
set $i 0
inputLoop:
arraySet $a $i inputNum
set $i add $i 1
if less $i $n goto inputLoop

set $i 0
outerLoop:
if less $i sub $n 1 goto outerBody
goto outputStart

outerBody:
set $min $i
set $j add $i 1

innerLoop:
if less $j $n goto innerBody
goto innerEnd

innerBody:
if less arrayGet $a $j arrayGet $a $min goto updateMin
goto innerNext

updateMin:
set $min $j

innerNext:
set $j add $j 1
goto innerLoop

innerEnd:
set $tmp arrayGet $a $i
arraySet $a $i arrayGet $a $min
arraySet $a $min $tmp
set $i add $i 1
goto outerLoop

outputStart:
set $i 0
outputLoop:
print arrayGet $a $i
printChar 32
set $i add $i 1
if less $i $n goto outputLoop
printChar 10
