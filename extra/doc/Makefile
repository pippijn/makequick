all:
	$(RM) -r latex
	doxygen
	$(MAKE) -C latex

view: all
	xpdf latex/refman.pdf
