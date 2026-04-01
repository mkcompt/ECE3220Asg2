#Kyle Compton (mkcompt)
 # ECE 3220
 #Assignment 2
 #04/04/2026
 #makefile

comp = gcc
compflags = -Wall -Wextra -g


asg2: asg2.c
	$(comp) $(compflags) asg2.c -o asg2 

clean:
	rm -f asg2