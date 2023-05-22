struct Animal {
    @private String sound;
}
extend Animal {
    @internal
    @static Animal new( String sound ) {
        Animal a = {};
        a.sound = sound;
        return a;
    }
    
    @internal
    void make_sound( !*!Animal self ) {
        print(self.sound);
    }
}

struct Dog {
    @private Animal animal;
}
extend Dog {
    @internal
    @static Dog new() {
        Dog dog = {};
        dog.animal = Animal.new("woof");
        return dog;
    }
    
    @internal
    *!Animal get_animal( !*!Dog self ) {
        return self.animal;
    }
}

void main() {
    Dog dog = Dog.new();
    dog.get_animal().make_sound();
}
