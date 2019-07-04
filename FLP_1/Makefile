# simplify-bkg
# xnecas24
# Klara Necasova

LOGIN=xnecas24
PROJ=simplify-bkg
ARCHIVEPROJ=flp-fun-$(LOGIN)
FILES=Makefile simplify-bkg.hs dir_in dir_ref it_is_ok.sh run_tests.sh readme-tests.md README

all:
	ghc --make simplify-bkg.hs -o $(PROJ)

pack: all
	zip -r $(ARCHIVEPROJ).zip $(FILES)

clean:
	rm -f simplify-bkg
	rm -f *.hi
	rm -f *.o
    
