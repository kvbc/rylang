









































:std = {
    :math = {
        :$Vector2 = {
            x i32;
            y i32;
        
            :add (self !*! $Vector2)i32 = {
                $Result = { ok bool; }
                break self.x + self.y;
            }
        }
    }
}

pos :std:math:$Vector2;
use :std:math:*;
pos :$Vector2;

:std:math:$Vector2:add():$Result;





:$Vector2 = { x i32; y i32; };
:#Color = {
    RED;
    GREEN;
    BLUE;

    :()opposite (color :#Color):#Color {
        break match(color) {
            RED => BLUE;
            GREEN => GREEN;
            BLUE => RED;
        }
    }
}

pos :$Vector2 = { .x = 0, .y = 0 };
clr :#Color = :#Color:RED;
opp_clr :#Color = :#Color:opposite(clr);

pos $Vector2 = { .x = 0, .y = 0 };
clr #Color = #Color:RED;
opp_clr #Color = #Color:opposite(clr);