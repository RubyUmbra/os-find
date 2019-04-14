EXE_FILE=find
OUT_DIR=out

.PHONY: all clean

all: ${EXE_FILE}
	rm -rf ${OUT_DIR}

${EXE_FILE}: ${OUT_DIR}/main.o ${OUT_DIR}/paths_stack.o ${OUT_DIR}/find_filters.o | ${OUT_DIR}
	cc -o $@ ${OUT_DIR}/main.o ${OUT_DIR}/paths_stack.o ${OUT_DIR}/find_filters.o

${OUT_DIR}/main.o: main.c paths_stack.h find_filters.h | ${OUT_DIR}
	cc -o $@ -c $<

${OUT_DIR}/paths_stack.o: paths_stack.c paths_stack.h | ${OUT_DIR}
	cc -o $@ -c $<

${OUT_DIR}/find_filters.o: find_filters.c find_filters.h | ${OUT_DIR}
	cc -o $@ -c $<

${OUT_DIR}:
	mkdir ${OUT_DIR}

clean:
	rm -rf ${OUT_DIR}
	rm -f find
