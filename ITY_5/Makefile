# Soubor: 	Makefile
# Popis: 	  Makefile pro 5. projekt predmetu ITY
# Autor:	  Klara Necasova, xnecas24

NAME=proj5

all:
	latex $(NAME).tex
	latex $(NAME).tex
	dvips $(NAME).dvi
	ps2pdf $(NAME).ps

clean:
	rm -f $(NAME).ps
	rm -f $(NAME).pdf
	rm -f $(NAME).log
	rm -f $(NAME).aux
	rm -f $(NAME).out
	rm -f $(NAME).blg 
	rm -f $(NAME).dvi
	rm -f $(NAME).te~