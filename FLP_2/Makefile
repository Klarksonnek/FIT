# Rubikova kostka
# xnecas24
# Klara Necasova

LOGIN=xnecas24
PROJ=flp18-log
ARCHIVEPROJ=flp-log-$(LOGIN)
FILES=Makefile cube.pl test1.in test2.in test3.in test4.in test5.in README

all:
	swipl -q -g main -o $(PROJ) -c cube.pl

pack: all
	zip -r $(ARCHIVEPROJ).zip $(FILES)

clean:
	rm -f $(PROJ)
