# RUN: rlc %s -o %t -i %stdl.ib --print-ir-on-failure=false 2>&1 --expect-fail | FileCheck %s

# CHECK: 8:1: error: No known type ref int in function declaration return type  

cls Asd:
	Int x

fun ret_x(Asd val) -> ref int:
	return val.x

fun main() -> Int:
	let var : Asd
	var.x = 10
	let refvar = ret_x(var)
	var.x = 20
	return refvar - 20

