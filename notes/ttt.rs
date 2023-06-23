Game var = [
    board [[char * 3] * 3]
    turn char;

    new[] => self = [
        board = [[' ' * 3] * 3];
        turn = ' '
    ]

    is_playing[me self] => bool = {
        break me.turn == 'x' || me.turn == 'y';
    }

    start[me self] => [] = {
        assert !me:is_playing[]
        me.turn = 'x';
    }

    next[me self] => [] = {
        assert me:is_playing[]
            
    }
]

main[] => [] = {
    Game game = Game:new[];

}