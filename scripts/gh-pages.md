# github pages

You can create github pages with doxygen by follow [this link](https://help.github.com/en/github/working-with-github-pages/creating-a-github-pages-site#creating-your-site) and [this link](https://gist.github.com/vidavidorra/548ffbcdae99d752da02).

First `stash` or `commit` all changes
```
git stash --include-untracked
```

Then, create empty `gh-pages` branch
```
./scripts/createGithubPages.sh
```
