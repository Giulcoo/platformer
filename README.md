## Platformer - Language, Compiler and Runner for Platform Games
Project for the course "Language, Compiler and Interpreter" of the University of Parma in the winter semester 2024/25. This tool lets developers define maps for platformer games and then play them.

---

### Platformer Language
#### Program
Each program consists of a screen setting, player and one or more worlds. The following code shows an example game.

```
screen(1600, 800, 100);

player("Start", 20, 40);

world("Start", 50, 40){
    block(0,0,500,200);
    checkpoint(400,50,20,20);
    platform(100, 200, 100, 5);
    block(500,30,300,200);
    finish(700, 110, 40, 80, "Level 2"); 
}

world("Level 2", 100, 50){
    block(0,0,300, 200);
    block(350,50,400, 200);
    finish(700, 130, 40, 80, "Start");
}
```
#### Screen
The screen object sets the width and height of the screen of the game and the zoom factor. The zoom is in percentage. So under 100 the camera zooms out and above 100 it zooms in.\
```screen(String: title, Number: width, Number: height, Number: zoom);```

#### Player
The player is the character that the user can move around. It is defined like the following code shows.
```player(String: initial world, Number: width, Number: height);```

#### World
Each world has the following attributes and inside the ```{ ... }``` section the game objects of a world is defined. Each world equals to one level of the game.\
```world(String: Name, Number: Player spawn x, Number: player spawn y){ ... }```

#### Game objects
The following code shows how to define game objects inside worlds. A game object can be a ```block```, a ```platform```, a ```spike``` or a ```checkpoint```.\
```gameobject(Number: x, Number: y, Number: width, Number: height);```

#### Special objects
In this section special game objects are defined. They can be added to the content of a world and have additional attributes to normal game objects.

The finish object can be used to mark a teleportation point to another world, like the following code shows.\
```finish(Number: x, Number: y, Number: width, Number: height, String: next world);```


---

### Run Platformer-code
Developers can write their programs in a txt-file. The following steps show how to run the written program.

#### Installation
First clone the repository and then install following C libraries
- Bison 
- Flex
- Raylib (install libraries: raylib raylib-devel)

Then for setup run the command ```make```. 

#### Compilation
If for example you write your Platformer code in the file ```platform_game.txt```, the command\
```make compile INPUT=platform_game```\
can be used, to compile the code.

#### Run the program
If the compilation of the program was successful run ```make run``` to play the created game.


---

### Game controls
To move the player left and right ```a``` and ```d``` are used and for jumping the key ```space```. To respawn at the last checkpoint the key ```r``` is used.