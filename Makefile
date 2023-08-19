OBJ = project4 p4diff
CFLAGS = -Wall -g
CC = gcc

executables: $(OBJ)
	
%: %.c
	$(CC) $(CFLAGS) $< -o $@

tests: test2 test3 test4 test5 test6 test7 test8 test9 test10

test2:
	cd project4tests ;\
	../project4 testprog2.c testprog2.args testprog2.in testprog2.out

test3:
	cd project4tests ;\
	../project4 testprog3.c testprog3.args testprog3.in testprog3.out

test4: test4a test4b test4c test4d test4e

test4a:
	cd project4tests ;\
	if ./../p4diff test4a_golden < test4a_in | diff - test4a_result >/dev/null; then \
		printf "\e[41mTest Failed\n\e[0m\n"; \
	else \
		printf "\e[42mTest Passed\e[0m\n"; \
	fi

test4b:
	cd project4tests ;\
	if ./../p4diff test4b_golden < test4b_in | diff - test4b_result >/dev/null; then \
		printf "\e[41mTest Failed\n\e[0m\n"; \
	else \
		printf "\e[42mTest Passed\e[0m\n"; \
	fi

test4c:
	cd project4tests ;\
	if ./../p4diff test4c_golden < test4c_in | diff - test4c_result >/dev/null; then \
		printf "\e[41mTest Failed\n\e[0m\n"; \
	else \
		printf "\e[42mTest Passed\e[0m\n"; \
	fi

test4d:
	cd project4tests ;\
	if ./../p4diff test4d_golden < test4d_in | diff - test4d_result >/dev/null; then \
		printf "\e[41mTest Failed\n\e[0m\n"; \
	else \
		printf "\e[42mTest Passed\e[0m\n"; \
	fi

test4e:
	cd project4tests ;\
	if ./../p4diff test4e_golden < test4e_in | diff - test4e_result >/dev/null; then \
		printf "\e[41mTest Failed\n\e[0m\n"; \
	else \
		printf "\e[42mTest Passed\e[0m\n"; \
	fi

test5:
	./project4 project4tests/testprog5.c project4tests/testprog5.args project4tests/testprog5.in project4tests/testprog5.out

test6:
	./project4 project4tests/testprog6.c project4tests/testprog6.args project4tests/testprog6.in project4tests/testprog6.out

test7:
	./project4 project4tests/testprog7.c project4tests/testprog7.args project4tests/testprog7.in project4tests/testprog7.out

test8:
	./project4 project4tests/testprog8.c project4tests/testprog8.args project4tests/testprog8.in project4tests/testprog8.out

test9:
	./project4 project4tests/testprog9.c project4tests/testprog9.args project4tests/testprog9.in project4tests/testprog9.out

test10:
	./project4 project4tests/testprog10.c project4tests/testprog10.args project4tests/testprog10.in project4tests/testprog10.out
