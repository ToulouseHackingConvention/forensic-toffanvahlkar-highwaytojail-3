all: flag export

flag: deb64-flag.qcow2

deb64-flag.qcow2: deb64.qcow2 upload_flag.sh
	rm -f import
	ln -s ../import
	qemu-img create -f qcow2 -b deb64.qcow2 deb64-flag.qcow2
	sudo ./upload_flag.sh

export: deb64.qcow2 deb64-flag.qcow2 start_vm.sh qemu-ifup.sh qemu-ifdown.sh ssh_config id_rsa id_rsa.pub
	rm -f export
	ln -s ../export
	rm -f build
	ln -s ../build
	cp deb64.qcow2 export/deb64-noflag.qcow2
	cp id_rsa.pub export/
	cp id_rsa export/
	sudo ./start_vm.sh

clean:
	rm -f import
	rm -f export
	rm -f build
	rm -f deb64-flag.qcow2

.PHONY: all flag export clean
