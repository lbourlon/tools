bins:
	mkdir -p bins

builds: bins bind messiah

aline:
	@echo "todo"

bind:
	cd $@/ && cargo build --release && mv target/release/$@ ../bins/

messiah:
	cd $@/ && make && mv $@ ../bins/

tests:
	mkdir -p test-results
	@cd messiah/ && make test && mv tests/results.xml ../test-results/messiah_results.xml


.PHONY: messiah tests builds aline bind
