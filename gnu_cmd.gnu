set xrange [-10:10]
set yrange [-100:100]
set terminal png
set output "func_graph.png"
plot "data.txt" title "function" with lines
exit
