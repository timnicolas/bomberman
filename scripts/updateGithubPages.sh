#!/bin/sh

GIT_REPO_URL="`git config --get remote.origin.url`"
TMP_DIR="/tmp/__code_docs"
DOXYFILE="scripts/Doxyfile"

rm -rf $TMP_DIR
git clone -b gh-pages GIT_REPO_URL $TMP_DIR

doxygen $DOXYFILE

cd $TMP_DIR
git add -A
git commit --amend -m "update docs" --no-verify
git push origin gh-pages -f
