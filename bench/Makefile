osx:
	gcc mach_current_time.c helpers.c mach_test.c -std=c99 \
		-Werror -o osx; ./osx; rm -rf osx

posix:
	@echo "OSTYPE: $$OSTYPE"; \
	gcc posix_current_time.c helpers.c posix_test.c -std=c99 \
		-Werror -o posix; ./posix; rm -rf posix
