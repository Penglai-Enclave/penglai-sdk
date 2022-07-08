all: dhrystone miniz norx  qsort sha512

dhrystone: dhrystone.c
	make  -f dhrystone.Makefile

miniz: miniz.c
	make  -f miniz.Makefile

norx: norx.c
	make  -f norx.Makefile

qsort: qsort.c
	make -f qsort.Makefile

sha512: sha512.c
	make  -f sha512.Makefile

clean:
	rm -f aes bigint dhrystone miniz norx primes qsort sha512 *.o
	rm -f src/aes src/bigint src/dhrystone src/miniz src/norx src/primes src/qsort src/sha512 src/*.o
