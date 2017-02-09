all: flag export

flag: allnightlong-flag.qcow2

allnightlong-flag.qcow2: allnightlong.qcow2 upload_flag.sh
	rm -f import
	ln -s ../import
	qemu-img create -f qcow2 -b allnightlong.qcow2 allnightlong-flag.qcow2
	./upload_flag.sh

export: allnightlong.qcow2 allnightlong-flag.qcow2 start_vm.sh qemu-ifup.sh qemu-ifdown.sh ssh_config id_rsa id_rsa.pub
	rm -f export
	ln -s ../export
	rm -f build
	ln -s ../build
	cp id_rsa.pub export/
	cp id_rsa export/
	./start_vm.sh

clean:
	rm -f import
	rm -f export
	rm -f build
	rm -f allnightlong-flag.qcow2

.PHONY: all flag export clean
