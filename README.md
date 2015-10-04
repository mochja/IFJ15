# IFJ15

HipChat room — https://unknown-team.hipchat.com/chat

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
