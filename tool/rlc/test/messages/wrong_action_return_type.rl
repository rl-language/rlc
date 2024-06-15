# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: error: User defined type clashes with implicit action type Ent

cls Ent:
    Int x

act action() -> Ent:
	return 5

