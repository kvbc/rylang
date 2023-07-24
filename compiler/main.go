package main

import "rylang/compiler/lexer"

func main() {
	lexer.Lex("main.ry", "if true then end my life")
}