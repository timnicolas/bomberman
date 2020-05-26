# bomberman [[42](https://www.42.fr/) project] [![Build Status](https://travis-ci.com/tnicolas42/bomberman.svg?branch=master)](https://travis-ci.com/tnicolas42/bomberman)

## 💣 The project

A 3d remake of the classic game Bomberman.

The project is fully written in C++ with the OpenGL framework.

### 📚 Rules

Bomberman is a series of video games from Hudson Soft where the player plays a bomber, the aim being to use bomb to protect youself from opponents and exit the level.

> View the full game rules in the in-game **help menu**.

![example game gif video here](/assets/GIFs/example.gif)

### ⌨ General controls

You can see and update all controls in the in-game **settings menu**.

![Control menu image here](/assets/GIFs/example.png)

Some hidden debug controls:

| Key  | Description |
| --- | --- |
| `F1` | Show the menu navigation shortcuts |
| `F3` | Show debugs infos (fps, nb of entities, ...) |
| `C` | Enable Free Flight Camera (only available with the [DEBUG flag](### Manual installation)) |
| `WASD` | Enable Free Flight Camera (only available with the [DEBUG flag](### Manual installation)) |

### 💾 save & load system

![save_menu image here](/assets/GIFs/example.png)

### 👩‍💻 Cheatcodes

You can enter cheatcode by pressing the `/` key.

| Command  | Description |
| --- | --- |
| `/help` [command, ...]  | Get general help or help on a command.  |
| `/clear` [history \| all \| lines]  | Clear lines or/and history.  |
| `/log` <type> <message>  | Log a message in the system console, `/log list` to get the list of types.  |
| `/tp` <x> <y>  | Teleport player to a given position (if possible). Use relative position with ~ (~-1, ~3, ...)<br>`/tp 3 ~`: Tp the player 3 block right to you.  |
| `/getbonus` <bonus ...>  | Get a bonus effect, `/getbonus list` to get the list of bonus.  |
| `/loop` <nb> <commands ...>  | Execute commands `nb` times, example:<br>`/loop 3 "/clear" "/getbonus life"`  |
| `/exec` <commands ...>  | Execute multiples commands, example:<br>`/exec "/clear" "/log info \"cleared\""`  |
| `/summon` <typename> <x> <y>  | Summon an entity at specified location, `/summon list` to get the list of entities   |
| `/unlock` <levelId ...>  | Unlock specified level  |
| `/rmbonus` <bonus, ...>  | Remove a bonus effect `/rmbonus list` to get the list of bonus  |
| `/restart`   | Restart the current level  |
| `/debug` [show\|hide\|reset] <elem ...>  | Show/Hide/Reset debug element, `/debug list` to get the list of elements, example:<br>`/debug show collider`  |
| `/volume` <type> <value>  | Change music/sound volume.  |

## 💻 Installation

Bomberman is compatible with **OSX🍎** and **Linux🐧**.

If you are on **OSX🍎** you can directly download the dmg build [here](/release/file.dmg).

### 👷 Manual installation

Clone the repository and install the dependencies
```bash
git clone https://github.com/tnicolas42/bomberman
cd bomberman
make install
```

Compile the project

```bash
make -j8
```

```bash
make -j8 DEBUG=1  # to compile in debug mode
```

Execute the game
 ```usage
usage: ./bomberman [-u] [--reset] [--reset-settings] [--reset-history] [--reset-saves]
	--reset --reset-all: reset all users data (settings, history and saves) before starting
	--reset-settings: reset all users settings before starting
	--reset-history: reset commands history before starting
	--reset-saves: reset all users saves before starting
	-u, --usage: show usage
```

### 🗜 Makefile and Build

If you need to generate a `.dmg` for osx
```
make create_dmg
```

To get more infos about the makefile commands
```bash
make help
```

### 📖 The code is fully documented with doxygen
Check the documentation [here](https://tnicolas42.github.io/bomberman)
