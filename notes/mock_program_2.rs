// 
// 
// my mock copy
// 
// 

std := import "std";

#macro Field := [
    w, h i32;
    s [...[...bool]];

    #static New[w, h i32] => Field = {
        s := std.malloc[typeof s, h];
        for i := 1; i <= h; i += 1 do
            s.(i) = std.malloc[[...bool], w];
        break [w = w, h = h, s = s];
    };

    Set[self *Field, x, y i32, b bool] => [] = {
        self.s.(y).(x) = b;
    };

    Alive[self *Field, x, y i32] => bool = {
        x += self.w;
        x %= self.w;
        y += self.h;
        y %= self.h;
        break self.s.(y).(x);
    };

    Next[self *Field, x, y i32] => bool = {
        alive := 0;
        for i := -1, i <= 1, i += 1 do
            for j := -1, j <= 1, j += 1 do
                if (j != 0 or i != 0) and self:Alive[x+i, y+j] do
                    alive += 1;
        break alive == 3 or alive == 2 and self:Alive[x, y];
    };
];

#macro Life := [
    a, b Field;
    w, h i32;

    #static New[w, h i32] => Life = {
        a := Field.New[w, h];
        for i := 0; i < (w * h / 4); i += 1 do
            a:Set[std.rand[w], std.rand[h], true];
        break [
            a = a, b = Field.New[w, h];
            w = w, h = h
        ];
    };

    Step[self *Life] => [] = {
        for y := 0; y < self.h; y += 1 do
            for x := 0; x < self.w; x += 1 do
                self.b:Set[x, y, self.a:Next[x, y]];
        self.a, self.b = self.b, self.a;
    };

    String[self *Life] => *!char = /* ... */
];

main[] => [] = {
    life := Life.New[40, 15];
    for i := 0; i < 300; i += 1 do {
        life:Step[];
        std.print["\x0c", life:String[]];
        std.sleep[1000 / 30];
    }
};

// 
// 
// go original
// 
// 

// An implementation of Conway's Game of Life.
package main

import (
	"bytes"
	"fmt"
	"math/rand"
	"time"
)

// Field represents a two-dimensional field of cells.
type Field struct {
	s    [][]bool
	w, h int
}

// NewField returns an empty field of the specified width and height.
func NewField(w, h int) *Field {
	s := make([][]bool, h)
	for i := range s {
		s[i] = make([]bool, w)
	}
	return &Field{s: s, w: w, h: h}
}

// Set sets the state of the specified cell to the given value.
func (f *Field) Set(x, y int, b bool) {
	f.s[y][x] = b
}

// Alive reports whether the specified cell is alive.
// If the x or y coordinates are outside the field boundaries they are wrapped
// toroidally. For instance, an x value of -1 is treated as width-1.
func (f *Field) Alive(x, y int) bool {
	x += f.w
	x %= f.w
	y += f.h
	y %= f.h
	return f.s[y][x]
}

// Next returns the state of the specified cell at the next time step.
func (f *Field) Next(x, y int) bool {
	// Count the adjacent cells that are alive.
	alive := 0
	for i := -1; i <= 1; i++ {
		for j := -1; j <= 1; j++ {
			if (j != 0 || i != 0) && f.Alive(x+i, y+j) {
				alive++
			}
		}
	}
	// Return next state according to the game rules:
	//   exactly 3 neighbors: on,
	//   exactly 2 neighbors: maintain current state,
	//   otherwise: off.
	return alive == 3 || alive == 2 && f.Alive(x, y)
}

// Life stores the state of a round of Conway's Game of Life.
type Life struct {
	a, b *Field
	w, h int
}

// NewLife returns a new Life game state with a random initial state.
func NewLife(w, h int) *Life {	
	a := NewField(w, h)
	for i := 0; i < (w * h / 4); i++ {
		a.Set(rand.Intn(w), rand.Intn(h), true)
	}
	return &Life{
		a: a, b: NewField(w, h),
		w: w, h: h,
	}
}

// Step advances the game by one instant, recomputing and updating all cells.
func (l *Life) Step() {
	// Update the state of the next field (b) from the current field (a).
	for y := 0; y < l.h; y++ {
		for x := 0; x < l.w; x++ {
			l.b.Set(x, y, l.a.Next(x, y))
		}
	}
	// Swap fields a and b.
	l.a, l.b = l.b, l.a
}

// String returns the game board as a string.
func (l *Life) String() string {
	var buf bytes.Buffer
	for y := 0; y < l.h; y++ {
		for x := 0; x < l.w; x++ {
			b := byte(' ')
			if l.a.Alive(x, y) {
				b = '*'
			}
			buf.WriteByte(b)
		}
		buf.WriteByte('\n')
	}
	return buf.String()
}

func main() {
	l := NewLife(40, 15)
	for i := 0; i < 300; i++ {
		l.Step()
		fmt.Print("\x0c", l) // Clear screen and print field.
		time.Sleep(time.Second / 30)
	}
}
