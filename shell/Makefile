root_dir	= ../../../
libtar_topdir	= ${root_dir}src/libtar
TEST_DIR	= t

TEST_FLAG	= -DDBUG_TEST 

LIBTAR_LIBS = ${libtar_topdir}/lib/libtar.a

CFLAGS    = -I.	\
			-I.. \
			-Iinclude \
			-I${libtar_topdir} \
			-I${libtar_topdir}/lib \
			-I${libtar_topdir}/compat \
			-DUNIT_TEST
    #        -DUNIT_TEST -D_GNU_SOURCE -D SYS64 -ggdb -DDEBUG -DDLDEBUG


main:main.o common.o  nzip.o ntar.o thread.o
	gcc ${CFLAGS} -pthread ${root_dir}src/main/main.o ${root_dir}src/common/common.o ${root_dir}src/common/ntar.o ${root_dir}src/common/nzip.o ${root_dir}src/common/thread.o -o main ${LIBTAR_LIBS} -lz

main.o:
	cd ${root_dir}src/main/ && $(MAKE)

common.o:
	cd ${root_dir}src/common/ && $(MAKE)

ntar.o:
	cd ${root_dir}src/common/ && $(MAKE)

nzip.o:
	cd ${root_dir}src/common/ && $(MAKE)

tap.o:
	cd ${root_dir}src/$(TEST_DIR) && $(MAKE)

thread.o:
	cd ${root_dir}src/common/ && $(MAKE)

clean:
	cd ${root_dir}src/common/ && $(MAKE) clean
	cd ${root_dir}src/main/ && $(MAKE) clean
	rm main
#	@-rm src/main/main.o src/common/common.o src/main/main main
#	@-rm src/$(TEST_DIR)/test.o
