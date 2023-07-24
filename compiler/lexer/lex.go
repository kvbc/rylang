package lexer

import (
	"fmt"
)

type context struct {
	id  string
	src string
}

func Lex(id, src string) {
	ctx := context {
		id: id,
		src: src,
	}
	lexContext(ctx)
}

func lexContext(ctx context) {
	fmt.Printf("[%s] Source = %s", ctx.id, ctx.src)
}