GCC ?= riscv64-unknown-linux-gnu-gcc
ARC ?= riscv64-unknown-elf-ar

# LIBS = miracl.a  -lm

MIRACL = mrcore.c mrarth0.c mrarth1.c mrarth2.c mrcurve.c mrxgcd.c \
		mrarth3.c mrjack.c mrbits.c mrmonty.c \
		mrsroot.c mrlucas.c mrrand.c
MIRACL_SRC = $(addprefix source/miracl/, $(MIRACL))
MIRARCL_OBJ = $(patsubst %.c,%.o, $(MIRACL_SRC))
MIR_CFLAGS = -c -O2 -I include/miracl -I/usr/lib

SM2 = SM2_sv.c SM3.c SM4.c SM4_Enc.c Random.c
SM2_SRC = $(addprefix source/, $(SM2))
SM2_OBJ = $(patsubst %.c,%.o, $(SM2_SRC))
SM2_CFLAGS = -c -I include -I include/miracl

TARCGET = ../libpenglai-enclave-gm.a miracl.a sm2.a

all: $(TARCGET)

$(MIRARCL_OBJ): %.o: %.c
	$(GCC) $(MIR_CFLAGS) -o $@ $<
$(SM2_OBJ): %.o: %.c
	$(GCC) $(SM2_CFLAGS) -o $@ $<
$(TARCGET): $(MIRARCL_OBJ) $(SM2_OBJ)
	$(ARC) crv $@ $^

miracl.a: $(MIRARCL_OBJ)
	$(ARC) crv $@ $^

sm2.a: $(SM2_OBJ)
	$(ARC) crv $@ $^

clean:
	-rm -f $(MIRARCL_OBJ) $(SM2_OBJ) $(TARCGET)
