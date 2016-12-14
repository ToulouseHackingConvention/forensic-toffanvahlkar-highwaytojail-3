IMG = forensic-toffanvahlkar-allnightlong-3

image:
	docker build -t $(IMG) .

build: image

export:
	mkdir -p export
	docker run --rm --entrypoint cat $(IMG) /home/builder/build/cryptolock > export/cryptolock

clean:
	rm -rf export

clean-all:
	-docker rmi $(IMG)

.PHONY: image build export clean clean-all
