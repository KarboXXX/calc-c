compiler := gcc
args := -g -Wall -lm
output := calc

calc: calculadora.c
	$(compiler) $(args) calculadora.c -o $(output)
