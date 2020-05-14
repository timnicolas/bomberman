#!/bin/sh

# to reset
# git branch -D gh-pages && git push -d origin gh-pages

if [ -z "`git rev-parse --verify gh-pages`" ]; then
	branch="`git rev-parse --abbrev-ref HEAD`"
	git checkout --orphan gh-pages
	git rm -rf .
	echo "# Github pages" > README.md
	git add .
	git commit -a -m "create gh-pages branch" --no-verify
	git push origin gh-pages
	git checkout $branch

	echo ""
	echo "To create github pages"
	echo "https://help.github.com/en/github/working-with-github-pages/creating-a-github-pages-site#creating-your-site"
fi

GIT_REPO_URL="`git config --get remote.origin.url`"
TMP_DIR="/tmp/__code_docs"
DOXYFILE="scripts/Doxyfile"

rm -rf $TMP_DIR
git clone -b gh-pages $GIT_REPO_URL $TMP_DIR

doxygen $DOXYFILE
cp scripts/index.html $TMP_DIR

git --git-dir $TMP_DIR/.git add -A
git --git-dir $TMP_DIR/.git commit -m "update docs" --no-verify
git --git-dir $TMP_DIR/.git push origin gh-pages -f

rm -rf $TMP_DIR
