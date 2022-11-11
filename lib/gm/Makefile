CC = riscv64-unknown-linux-gnu-gcc
AR = riscv64-unknown-elf-ar

# LIBS = miracl.a  -lm

MIRACL = mrcore.c mrarth0.c mrarth1.c mrarth2.c mrcurve.c mrxgcd.c \
		mrarth3.c mrjack.c mrbits.c mrmonty.c \
		mrsroot.c mrlucas.c mrrand.c
MIRACL_SRC = $(addprefix source/miracl/, $(MIRACL))
MIRARL_OBJ = $(patsubst %.c,%.o, $(MIRACL_SRC))
MIR_CFLAGS = -c -O2 -I include/miracl -I/usr/lib

SM2 = SM2_sv.c SM3.c SM4.c SM4_Enc.c Random.c
SM2_SRC = $(addprefix source/, $(SM2))
SM2_OBJ = $(patsubst %.c,%.o, $(SM2_SRC))
SM2_CFLAGS = -c -I include -I include/miracl

ALL: miracl.a sm2.a

miracl.a: $(MIRARL_OBJ)
	$(AR) crv $@ $^

$(MIRARL_OBJ): %.o: %.c
	$(CC) $(MIR_CFLAGS) -o $@ $<

sm2.a: $(SM2_OBJ)
	$(AR) crv $@ $^

$(SM2_OBJ): %.o: %.c
	$(CC) $(SM2_CFLAGS) -o $@ $<

clean:
	-rm -f *.o *.a
	-rm -f source/miracl/*.o source/*.o
