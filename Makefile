IMG = forensic-toffanvahlkar-allnightlong-3

all: build export

build: image flag

image:
	sudo docker build -t $(IMG) .

flag:
	make -C vm/ flag

export:
	mkdir -p build
	mkdir -p export
	sudo docker run --rm --entrypoint cat $(IMG) /home/builder/build/cryptolock > build/cryptolock
	make -C vm/ export

clean:
	rm -rf build
	rm -rf export

clean-all: clean
	make -C vm/ clean
	-sudo docker rmi $(IMG)

.PHONY: all build image flag export clean clean-all
