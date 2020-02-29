submission_files=lab3a.c ext2_fs.h Makefile README
default: lab3a
lab3a: lab3a.c
	gcc -Wall -Wextra $^ -o $@
dist: $(submission_files)
	tar -czvf lab3a-205117980.tar.gz $^
clean:
	rm -f lab3a *.tar.gz
