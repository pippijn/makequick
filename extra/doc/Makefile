WEB := $(addsuffix .tex,$(addprefix web/,$(basename $(notdir $(wildcard ../web/makepp/*.pod)))))
OUT := doxygen

all: refman.pdf

refman.pdf: refman.tex $(OUT) $(WEB) doxygen.sty
	rubber --pdf -Wall refman.tex
	rubber --clean refman.tex

$(OUT): Doxyfile Makefile
	$(RM) -r $@
	touch nop.tex
	doxygen
	$(RM) nop.tex
	sed -i -e 's|^\\input{|&$(OUT)/|' $@/refman.tex
	sed -i -e 's|\$$<\$$|\\textless{}|g' $@/*.tex
	sed -i -e 's|\$$>\$$|\\textgreater{}|g' $@/*.tex

view: all
	xpdf refman.pdf

web/%.tex: pod2latex ../web/makepp/%.pod
	./$+ $@
