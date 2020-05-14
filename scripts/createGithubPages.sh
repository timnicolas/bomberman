#!/bin/sh

if [ ! -z "`git rev-parse --verify gh-pages`" ]; then
	echo "gh-pages branch already exist"
	echo "git branch -D gh-pages"
	echo "git push -d origin gh-pages"
	exit 1
fi

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

exit 0
