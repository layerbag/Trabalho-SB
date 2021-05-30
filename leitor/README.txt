Para rodar o programa, execute:

$  gcc leitor.c -o leitor
$  ./leitor < teste.blp > teste.S
$  gcc main.c teste.S -o prog
$  ./prog
