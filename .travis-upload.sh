
GITDATE="`git show -s --date=short --format='%ad' | sed 's/-//g'`"
GITREV="`git show -s --format='%h'`"

if [ "$TRAVIS_OS_NAME" = "linux" -o -z "$TRAVIS_OS_NAME" ]; then
    REV_NAME="citra-${GITDATE}-${GITREV}-linux-amd64"
    UPLOAD_DIR="/citra/nightly/linux-amd64"
    mkdir "$REV_NAME"

    cp build/src/citra/citra "$REV_NAME"
    cp build/src/citra_qt/citra-qt "$REV_NAME"
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    REV_NAME="citra-${GITDATE}-${GITREV}-osx-amd64"
    UPLOAD_DIR="/citra/nightly/osx-amd64"
    mkdir "$REV_NAME"

    brew install lftp
    cp build/src/citra/Release/citra "$REV_NAME"
    cp -r build/src/citra_qt/Release/citra-qt.app "$REV_NAME"

    # move qt libs into app bundle for deployment
    # This is the issue....
    $(brew --prefix)/opt/qt5/bin/macdeployqt "${REV_NAME}/citra-qt.app"

    # move SDL2 libs into folder for deployment
    # This does nothing... macdeployqt already handles this..
    # dylibbundler -b -x "${REV_NAME}/citra" -cd -d "${REV_NAME}/libs" -p "@executable_path/libs/"
fi

# Copy documentation
cp license.txt "$REV_NAME"
cp README.md "$REV_NAME"

ARCHIVE_NAME="${REV_NAME}.tar.xz"
tar -cJvf "$ARCHIVE_NAME" "$REV_NAME"

# Personal build to check out what's going on with Travis-CI and Mac OS X (not MacOS, that isn't out yet).
# lftp -c "open -u citra-builds,$BUILD_PASSWORD sftp://builds.citra-emu.org; set sftp:auto-confirm yes; put -O '$UPLOAD_DIR' '$ARCHIVE_NAME'"
# https://gist.github.com/domenic/ec8b0fc8ab45f39403dd
TARGET_BRANCH="builds"

REPO=`git config remote.origin.url`
SSH_REPO=${REPO/https:\/\/github.com\//git@github.com:}
SHA=`git rev-parse --verify HEAD`

# Clone the existing gh-pages for this repo into out/
# Create a new empty branch if gh-pages doesn't exist yet (should only happen on first deply)
git clone $REPO out
cd out
git checkout $TARGET_BRANCH || git checkout --orphan $TARGET_BRANCH
cd ..
rm -rf out/**/* || exit 0
mv $ARCHIVE_NAME out
cd out
git config user.name "Travis CI"
git config user.email "spam@example.com"
git add .
git commit -m "Deploy to Build: ${SHA}"

# Get the deploy key by using Travis's stored variables to decrypt deploy_key.enc
ENCRYPTED_KEY_VAR="encrypted_${ENCRYPTION_LABEL}_key"
ENCRYPTED_IV_VAR="encrypted_${ENCRYPTION_LABEL}_iv"
ENCRYPTED_KEY=${!ENCRYPTED_KEY_VAR}
ENCRYPTED_IV=${!ENCRYPTED_IV_VAR}
openssl aes-256-cbc -K $ENCRYPTED_KEY -iv $ENCRYPTED_IV -in deploy_key.enc -out deploy_key -d
chmod 600 deploy_key
eval `ssh-agent -s`
ssh-add deploy_key

# Now that we're all set up, we can push.
git push $SSH_REPO $TARGET_BRANCH


