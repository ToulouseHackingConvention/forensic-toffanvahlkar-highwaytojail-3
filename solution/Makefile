all: solve

tmp:
	mkdir -p tmp

export:
	mkdir -p export

res:
	-ln -s ../../solution/export res

solve: tmp export res
	bin/dump_analyse

clean:
	rm -rf res
	rm -rf tmp

clean-all: clean
	rm -rf export

.PHONY: all solve clean clean-all
