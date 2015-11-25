# IFJ15

[![Build Status](https://travis-ci.com/mochja/IFJ15.svg?token=R1xPH8Tds42vtS3xNggs&branch=develop)](https://travis-ci.com/mochja/IFJ15)

## Style Guide

HipChat room — https://unknown-team.hipchat.com/chat

```C
#define ONE 1

typedef struct {
    int a;
} my_type_t; // alebo namespacovane tu_my_type_t;

void my_function(const char *s)
{
  char *c; // * pred nazov premennej, dovoluje potom char *c, *d, *e; miesto char* c, *d, *e;
  // jeden prazdny riadok za definiciou lok, premennych.
  if (true) {
    for (int i = 0; i < 10; i++) {
        while (1) {
            break;
        }
    }
  } else if ((1 > 2) && (3 == 2)) {
    // ...
  }
}

void __some_internal_fn()
{
  // ...
}
```

Naštudovať [branching model](http://nvie.com/posts/a-successful-git-branching-model/) a taktiež ako písať [commit messages](http://chris.beams.io/posts/git-commit/).

----

V jednoduchosti ak budete pracovat na niecom vzdy vo vlastnej branchi, najlepsie ak si vytvorite branch s prefixom svojho mena, eg.
```sh
git checkout -b jan/dement-x-2
```
Nasledne zmeny potom posunut do pull requestu, kde ich prejdeme a nasledne mergnem do development branchu.
**Nerobte žiadne commity/merge do master a develop branchu!!**

----

Projekt využíva [EditorConfig](http://editorconfig.org/#download)

## How to set up project workspace in sb3

https://www.sublimetext.com/3

https://packagecontrol.io/installation - (https://www.youtube.com/watch?v=ioRbV7fQkdU)

`ctrl` + `shift` + `p` :arrow_right: Install package :arrow_right: EditorConfig

![image](https://cloud.githubusercontent.com/assets/1218946/10567440/6bdf3202-7605-11e5-8ab5-500c0ba8daf9.png)

![image](https://cloud.githubusercontent.com/assets/1218946/10567471/31ffee72-7606-11e5-8748-5cb57d9825fb.png)

![image](https://cloud.githubusercontent.com/assets/1218946/10567482/6e2b24f2-7606-11e5-8d82-f86f9250334a.png)
