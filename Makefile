CFLAGS ?=	-Wall -Werror -O2 -pipe

all: rigg

clean:
	rm rigg

readme:
	mandoc -T markdown rigg.1 > README.md
