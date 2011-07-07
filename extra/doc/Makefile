WEB := $(addsuffix .tex,$(addprefix web/,$(basename $(notdir $(wildcard ../web/makepp/*.pod)))))

all: refman.pdf

refman.pdf: refman.tex latex $(WEB) doxygen.sty
	rubber --pdf -Wall refman.tex
	rubber --clean refman.tex

latex: Doxyfile Makefile
	$(RM) -r $@
	doxygen
	sed -i -e 's|^\\input{|&latex/|' $@/refman.tex
	sed -i -e 's|\$$<\$$|\\textless{}|g' $@/*.tex
	sed -i -e 's|\$$>\$$|\\textgreater{}|g' $@/*.tex

view: all
	xpdf refman.pdf

web/%.tex: pod2latex ../web/makepp/%.pod
	./$+ $@
