builds: messiah

aline:
	@echo "todo"

messiah:
	mkdir -p bins
	cd messiah/ && make && mv messiah ../bins/

tests:
	mkdir -p test-results
	@cd messiah/ && make test && mv tests/results.xml ../test-results/messiah_results.xml


.PHONY: messiah tests builds aline
