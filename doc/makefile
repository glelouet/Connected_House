.PHONY: all clean

all: moteino.pdf

%.pdf: %.tex
	pdflatex $< && pdflatex $<

clean:
	@ rm -rf *.aux *.log *.maf *.mtc *.mtc* *.out *.toc 
