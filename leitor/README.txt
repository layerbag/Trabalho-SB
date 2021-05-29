Para rodar o programa, execute:

$  gcc leitor.c -o leitor
$  ./leitor < prog.blp > teste.S
$  gcc main.c teste.S -o prog
$  ./prog