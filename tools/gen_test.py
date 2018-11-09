#!/usr/bin/python3

import re
import sys
import os


TEST_FUNC_HEADER = """#include <stdio.h>
#include <stdbool.h>

static inline int assert(bool exp, char *func_name)
{
	printf("%s => ", func_name);
	if (exp) {
		printf("\\e[1;32mOK\\e[m\\n");
	} else {
		printf("\\e[1;31mNG\\e[m\\n");
	}

	return (!exp) ? 1 : 0;
}
"""


def main():

    reg_func = re.compile(
        r"^\s*(?P<ret_type>int)\s+(?P<func_name>test_\w+)\s*\((?P<parameters>.*)\)\s*/\*\s*(?P<args>([\w\s,]*?)\s*)\*/\s*/\*\s*(?P<expected>\w+)\s*\*/$")

    if len(sys.argv) < 2:
        print("Usage: {} [directory containing tests]".format(sys.argv[0]))
        sys.exit(1)

    dirs = sys.argv[1:]

    c_files = []

    for d in dirs:
        c_files += [os.path.join(d, f) for f in os.listdir(d)
                    if os.path.splitext(f)[1] == '.c']

    func_body = ""
    func_prototype = ""

    for cfile in c_files:

        with open(cfile, mode='rt') as f:

            line = f.readline()

            while line:

                m = reg_func.search(line)

                if m:
                    func_prototype += """{} {}({});\n""".format(
                        m.group('ret_type').strip(), m.group('func_name').strip(), m.group('parameters').strip())
                    func_body += """	ret += assert({}({}) == {}, "{}");\n""".format(
                        m.group('func_name').strip(), m.group('args').strip(),
                        m.group('expected').strip(), m.group('func_name').strip())

                line = f.readline()

    print(TEST_FUNC_HEADER)
    print(func_prototype)

    print("int main(void)\n{\n\tint ret = 0;")
    print(func_body)
    print("\treturn ret;\n}")


if __name__ == '__main__':
    main()
