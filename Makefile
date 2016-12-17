IMG = forensic-toffanvahlkar-allnightlong-3

image:
	sudo docker build -t $(IMG) .

build: image

export:
	mkdir -p export
	sudo docker run --rm --entrypoint cat $(IMG) /home/builder/build/cryptolock > export/cryptolock

clean:
	rm -rf export

clean-all:
	sudo docker rmi $(IMG)

.PHONY: image build export clean clean-all
