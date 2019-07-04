# Soubor: 	Makefile
# Popis: 		Makefile pro 4. projekt predmetu ITY
# Autor:		Klara Necasova, xnecas24

NAME=proj4

all:
	latex $(NAME).tex
	bibtex $(NAME)
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps

clean:
	rm -f $(NAME).ps
	rm -f $(NAME).pdf
	rm -f $(NAME).log
	rm -f $(NAME).aux
	rm -f $(NAME).out
	rm -f $(NAME).blg 
	rm -f $(NAME).bbg
	rm -f $(NAME).bbl 
	rm -f $(NAME).dvi
	rm -f $(NAME).te~