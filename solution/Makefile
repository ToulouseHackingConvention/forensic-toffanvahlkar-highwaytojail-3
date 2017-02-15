all: solve

solve:
	ln -s ../../solution/export res
	./extract.sh
	echo "Please upload the potential keys on the VM and then use crack.sh to test them."
	# ./vm_solve.sh

clean:
	rm -rf res
	rm -rf out

.PHONY: all solve clean
