builds: messiah

aline:
	@echo "todo"

messiah:
	@cd messiah/ && make && mv messiah ../bins/

tests:
	@cd messiah/ && make test


.PHONY: messiah tests builds aline
