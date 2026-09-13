# RandomAnime

Minimal CLI anime list manager written in C.

## Dependencies

* gcc
* make
* glibc

# 1. Installation

Clone the project, and run

```
$ make
$ make install clean
```

This installs `ra` to:

```
~/.local/bin/ra
```

Make sure `~/.local/bin` is in your PATH.

```
echo $PATH | grep -q "$HOME/.local/bin" && echo "Yes, it is in PATH" || echo "No, it is NOT in PATH"
```

If it's not in PATH, go to `$HOME/.bashrc` and add

```
export PATH="$PATH:$HOME/.local/bin"
```

# 2. Usage

Run without arguments to get a random anime from your list:

```
$ ra
```

Add an anime:

```
$ ra add "anime name"
```

Query your anime list:

```
$ ra query "anime name"
```

Remove an anime:

```
$ ra remove "anime name"
```

## Examples

```
$ ra add "Samurai Champloo"
Samurai Champloo is added succesfully to the anime list

$ ra query "samurai"
Samurai Champloo

$ ra remove "Samurai Champloo"
Samurai Champloo successfully removed.
```

# 3. Anime List

The anime list is stored automatically at:

```
~/Projects/C/RandomAnime/AnimeList.txt
```

The program creates and manages the file automatically.
