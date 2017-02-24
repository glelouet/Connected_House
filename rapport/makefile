.PHONY: all clean

all: moteino.pdf

%.pdf: %.tex
	pdflatex $<

clean:
	@ rm -rf *.aux *.log *.out
