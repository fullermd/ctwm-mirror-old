# This just shortcuts stuff through to cmake
all build ctwm man man-html man-all install clean: build/Makefile
	( cd build && ${MAKE} ${@} )

build/Makefile cmake: CMakeLists.txt
	( mkdir -p build && cd build && \
		cmake -DCMAKE_C_FLAGS:STRING="${CFLAGS}" ${CMAKE_EXTRAS} .. )

allclean distclean:
	rm -rf build/*



#
# The below targets are mostly only of interest to developers
#


# Reindent files
indent:
	astyle -n --options=tools/ctwm.astyle *.h *.c


# Build documentation files
# STYLE is also HTMLable, but there's no reason to do it by default
DOC_FILES=README.html CHANGES.html
docs: ${DOC_FILES}
docs_clean doc_clean:
	rm -f ${DOC_FILES}

.SUFFIXES: ${.SUFFIXES} .html .md
.md.html:
	multimarkdown -ao ${@} ${<}


# asciidoc files
adocs:
	(cd doc && make all_set_version)
adocs_pregen:
	(cd doc && make all)
adoc_clean:
	(cd doc && make clean)


#
# Pre-build some files for tarballs
#
GEN=gen
${GEN}:
	mkdir -p ${GEN}

# All the generated source files
_RELEASE_FILES=gram.tab.c gram.tab.h lex.c version.c.in ctwm.1 ctwm.1.html
RELEASE_FILES=${_RELEASE_FILES:%=${GEN}/%}

# Build those, the .html versions of the above docs, and the HTML/man
# versions of the manual
release_files: ${GEN} build/MKTAR_GENFILES ${RELEASE_FILES} ${DOC_FILES}
release_clean: doc_clean adoc_clean
	rm -rf ${GEN}

# Stuff for thunking to cmake
build/MKTAR_GENFILES: build/Makefile
	(cd build ; make mktar_genfiles)

# The config grammar
${GEN}/gram.tab.c: ${GEN}/gram.tab.h
${GEN}/gram.tab.h: ${GEN} gram.y build/MKTAR_GENFILES
	cp build/gram.tab.[ch] ${GEN}/

${GEN}/lex.c: ${GEN} lex.l build/MKTAR_GENFILES
	cp build/lex.c ${GEN}/

# Setup version file
${GEN}/version.c.in: ${GEN} version.c.in .bzr/checkout/dirstate
	tools/rewrite_version_bzr.sh < version.c.in > ${GEN}/version.c.in

# Generate pregen'd manuals
${GEN}/ctwm.1: doc/ctwm.1
${GEN}/ctwm.1.html: doc/ctwm.1.html
${GEN}/ctwm.1 ${GEN}/ctwm.1.html:
	cp doc/ctwm.1 doc/ctwm.1.html ${GEN}/
doc/ctwm.1 doc/ctwm.1.html:
	(cd doc && make clean all)


# Thunk through to gen'ing tarball
tar:
	tools/mk_tar.sh
