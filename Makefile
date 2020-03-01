# NAME: Di Wu,Jingnong Qu
# EMAIL: xiaowu200031@gmail.com,andrewqu2000@g.ucla.edu
# ID: 205117980,805126509

submission_files=lab3a.c ext2_fs.h Makefile README
default: lab3a
lab3a: lab3a.c
	gcc -Wall -Wextra $^ -o $@
dist: $(submission_files)
	tar -czvf lab3a-205117980.tar.gz $^
clean:
	rm -f lab3a *.tar.gz
