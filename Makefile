IMG = forensic-toffanvahlkar-allnightlong-3

all: build export

build:
	docker build -t $(IMG) .

export: build
	mkdir -p export
	docker run --rm --entrypoint cat $(IMG) /home/builder/build/cryptolock > export/cryptolock

clean:
	rm -rf export

clean-all: clean
	-docker rmi $(IMG)

.PHONY: all build export clean clean-all
