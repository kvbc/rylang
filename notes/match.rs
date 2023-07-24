#Error = {
    A;
    B;
    C;
    D;
    E;
    F;
};

get_error( ... ) #Error {
    ...
}

main() void {
    err #Error = get_error(...);
    
    match( err ) {
        #Error:A => print("A");
        #Error:B => print("B");
        #Error:C => print("C");
        #Error:D => print("D");
        #Error:E => print("E");
        #Error:F => print("F");
    };

    // exhaustiveness

      if( err == #Error:A ) { print("A"); }
    elif( err == #Error:B ) { print("B"); }
    elif( err == #Error:C ) { print("C"); }
    elif( err == #Error:D ) { print("D"); }
    elif( err == #Error:E ) { print("E"); }
    elif( err == #Error:F ) { print("F"); }
}
