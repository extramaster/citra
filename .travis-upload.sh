if [ "$TRAVIS_BRANCH" != "builds" ]; then
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
        $(brew --prefix)/opt/qt5/bin/macdeployqt "${REV_NAME}/citra-qt.app" -executable="${REV_NAME}/citra-qt.app/Contents/MacOS/citra-qt"

        # move SDL2 libs into folder for deployment
        dylibbundler -b -x "${REV_NAME}/citra" -cd -d "${REV_NAME}/libs" -p "@executable_path/libs/"

        # Make the changes to make the citra-qt app standalone (i.e. not dependent on the current brew installation).
        # To do this, the absolute references to each and every QT framework must be re-written to point to the local frameworks
        # (in the Contents/Frameworks folder).
        # The "install_name_tool" is used to do so.

        # Coreutils is a hack to coerce Homebrew to point to the absolute Cellar path (symlink dereferenced). i.e:
        # ls -l /usr/local/opt/qt5:: /usr/local/opt/qt5 -> ../Cellar/qt5/5.6.1-1
        # grealpath ../Cellar/qt5/5.6.1-1:: /usr/local/Cellar/qt5/5.6.1-1
        brew install coreutils

        REV_NAME_ALT=$REV_NAME/
        # grealpath is located in coreutils, there is no "realpath" for OS X :(
        QT_BREWS_PATH=$(grealpath "$(brew --prefix qt5)")
        BREW_PATH=$(brew --prefix)
        QT_VERSION_NUM=5

        $BREW_PATH/opt/qt5/bin/macdeployqt "${REV_NAME_ALT}citra-qt.app" \
            -executable="${REV_NAME_ALT}citra-qt.app/Contents/MacOS/citra-qt"

        # These are the files that macdeployqt packed into Contents/Frameworks/ - we don't want those, so we replace them.
        declare -a macos_libs=("QtCore" "QtWidgets" "QtGui" "QtOpenGL" "QtPrintSupport")

        for macos_lib in "${macos_libs[@]}"
        do
            SC_FRAMEWORK_PART=$macos_lib.framework/Versions/$QT_VERSION_NUM/$macos_lib
            # Replace macdeployqt versions of the Frameworks with our own (from /usr/local/opt/qt5/lib/)
            cp "$BREW_PATH/opt/qt5/lib/$SC_FRAMEWORK_PART" "${REV_NAME_ALT}citra-qt.app/Contents/Frameworks/$SC_FRAMEWORK_PART"

            # Replace references within the embedded Framework files with "internal" versions.
            for macos_lib2 in "${macos_libs[@]}"
            do
                # Since brew references both the non-symlinked and symlink paths of QT5, it needs to be duplicated.
                # /usr/local/Cellar/qt5/5.6.1-1/lib and /usr/local/opt/qt5/lib both resolve to the same files.
                # So the two lines below are effectively duplicates when resolved as a path, but as strings, they aren't.
                RM_FRAMEWORK_PART=$macos_lib2.framework/Versions/$QT_VERSION_NUM/$macos_lib2
                install_name_tool -change \
                    $QT_BREWS_PATH/lib/$RM_FRAMEWORK_PART \
                    @executable_path/../Frameworks/$RM_FRAMEWORK_PART \
                    "${REV_NAME_ALT}citra-qt.app/Contents/Frameworks/$SC_FRAMEWORK_PART"
                install_name_tool -change \
                    "$BREW_PATH/opt/qt5/lib/$RM_FRAMEWORK_PART" \
                    @executable_path/../Frameworks/$RM_FRAMEWORK_PART \
                    "${REV_NAME_ALT}citra-qt.app/Contents/Frameworks/$SC_FRAMEWORK_PART"
            done
        done

        # Handles `This application failed to start because it could not find or load the Qt platform plugin "cocoa"`
        # Which manifests itself as:
        # "Exception Type: EXC_CRASH (SIGABRT) | Exception Codes: 0x0000000000000000, 0x0000000000000000 | Exception Note: EXC_CORPSE_NOTIFY"
        # There may be more dylibs needed to be fixed...
        declare -a macos_plugins=("Plugins/platforms/libqcocoa.dylib")

        for macos_lib in "${macos_plugins[@]}"
        do
            install_name_tool -id @executable_path/../$macos_lib "${REV_NAME_ALT}citra-qt.app/Contents/$macos_lib"
            for macos_lib2 in "${macos_libs[@]}"
            do
                RM_FRAMEWORK_PART=$macos_lib2.framework/Versions/$QT_VERSION_NUM/$macos_lib2
                install_name_tool -change \
                    $QT_BREWS_PATH/lib/$RM_FRAMEWORK_PART \
                    @executable_path/../Frameworks/$RM_FRAMEWORK_PART \
                    "${REV_NAME_ALT}citra-qt.app/Contents/$macos_lib"
                install_name_tool -change \
                    "$BREW_PATH/opt/qt5/lib/$RM_FRAMEWORK_PART" \
                    @executable_path/../Frameworks/$RM_FRAMEWORK_PART \
                    "${REV_NAME_ALT}citra-qt.app/Contents/$macos_lib"
            done
        done

        for macos_lib in "${macos_libs[@]}"
        do
            # Debugging info for Travis-CI
            otool -L "${REV_NAME_ALT}citra-qt.app/Contents/Frameworks/$macos_lib.framework/Versions/$QT_VERSION_NUM/$macos_lib"
        done

        # Make the citra-qt.app application launch a debugging terminal.
        # Store away the actual binary
        mv citra-qt.app/Contents/MacOS/citra-qt citra-qt.app/Contents/MacOS/citra-qt-bin

        cat > citra-qt.app/Contents/MacOS/citra-qt <<EOL
#!/usr/bin/env bash
cd "\`dirname "\$0"\`"
chmod +x citra-qt-bin
open citra-qt-bin
EOL
        # Content that will serve as the launching script for citra (within the .app folder)

        # Make the launching script executable
        chmod +x citra-qt.app/Contents/MacOS/citra-qt

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
    git add . -A
    git commit -m "Deploy to Build: ${SHA}"

    # Get the deploy key by using Travis's stored variables to decrypt deploy_key.enc
    ENCRYPTED_KEY_VAR="encrypted_${ENCRYPTION_LABEL}_key"
    ENCRYPTED_IV_VAR="encrypted_${ENCRYPTION_LABEL}_iv"
    ENCRYPTED_KEY=${!ENCRYPTED_KEY_VAR}
    ENCRYPTED_IV=${!ENCRYPTED_IV_VAR}
    openssl aes-256-cbc -K $ENCRYPTED_KEY -iv $ENCRYPTED_IV -in ../deploy_key.enc -out deploy_key -d
    chmod 600 deploy_key
    eval `ssh-agent -s`
    ssh-add deploy_key

    # Now that we're all set up, we can push.
    git push $SSH_REPO $TARGET_BRANCH

fi
