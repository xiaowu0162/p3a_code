submission_files=lab3a.c Makefile README
default: lab3a
lab3a: lab3a.c
	gcc $^ -o $@
dist: $(submission_files)
	tar -czvf lab3a-205117980.tar.gz $^
clean:
	rm -f lab3a *.tar.gz
