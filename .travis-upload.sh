if [ "$TRAVIS_BRANCH" = "master" ]; then
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
        $(brew --prefix)/opt/qt5/bin/macdeployqt "${REV_NAME}/citra-qt.app"

        # move SDL2 libs into folder for deployment
        dylibbundler -b -x "${REV_NAME}/citra" -cd -d "${REV_NAME}/libs" -p "@executable_path/libs/"

        # Make the changes to make the citra-qt app standalone (i.e. not dependent on the current brew installation).
        # To do this, the absolute references to each and every QT framework must be re-written to point to the local frameworks (in the Contents/Frameworks folder).
        # The "install_name_tool" is used to do so.

        # Coreutils is a hack to coerce Homebrew to point to the absolute Cellar path (symlink dereferenced). i.e:
        # ls -l /usr/local/opt/qt5:: /usr/local/opt/qt5 -> ../Cellar/qt5/5.6.1-1
        # grealpath ../Cellar/qt5/5.6.1-1:: /usr/local/Cellar/qt5/5.6.1-1
        brew install coreutils

        # grealpath is located in coreutils, there is no "realpath" for OS X :(
        QT_BREWS_PATH=$(grealpath "$(brew --prefix qt5)")
        BREW_PATH=$(brew --prefix)
        QT_VERSION_NUM=5

        $BREW_PATH/opt/qt5/bin/macdeployqt "citra-qt.app" -executable="citra-qt.app/Contents/MacOS/citra-qt"

        # These are the files that macdeployqt packed into Contents/Frameworks/ - we don't want those, so we replace them.
        declare -a macos_libs=("QtCore" "QtWidgets" "QtGui" "QtOpenGL" "QtPrintSupport")

        for macos_lib in "${macos_libs[@]}"
        do
               # Replace macdeployqt versions of the Frameworks with our own (from /usr/local/opt/qt5/lib/)
               cp "$BREW_PATH/opt/qt5/lib/$macos_lib.framework/Versions/$QT_VERSION_NUM/$macos_lib" "citra-qt.app/Contents/Frameworks/$macos_lib.framework/Versions/$QT_VERSION_NUM/$macos_lib"

               # Replace references within the embedded Framework files with "internal" versions.
               for macos_lib2 in "${macos_libs[@]}"
               do
                   # Since brew references both the non-symlinked and symlink paths of QT5, it needs to be duplicated.
                   # /usr/local/Cellar/qt5/5.6.1-1/lib and /usr/local/opt/qt5/lib both resolve to the same files. So the two lines are effectively duplicates when resolved as a path, but as strings, they aren't.
                   install_name_tool -change $QT_BREWS_PATH/lib/$macos_lib2.framework/Versions/$QT_VERSION_NUM/$macos_lib2 @executable_path/../Frameworks/$macos_lib2.framework/Versions/$QT_VERSION_NUM/$macos_lib2 "citra-qt.app/Contents/Frameworks/$macos_lib.framework/Versions/$QT_VERSION_NUM/$macos_lib"
                   install_name_tool -change "$BREW_PATH/opt/qt5/lib/$macos_lib2.framework/Versions/$QT_VERSION_NUM/$macos_lib2" @executable_path/../Frameworks/$macos_lib2.framework/Versions/$QT_VERSION_NUM/$macos_lib2 "citra-qt.app/Contents/Frameworks/$macos_lib.framework/Versions/$QT_VERSION_NUM/$macos_lib"
               done
        done

        # Handles the "This application failed to start because it could not find or load the Qt platform plugin "cocoa"" which masks itself as the ""
        # Which manifests itself as "Exception Type:        EXC_CRASH (SIGABRT) Exception Codes:       0x0000000000000000, 0x0000000000000000 Exception Note:        EXC_CORPSE_NOTIFY"
        # There may be more dylibs needed to be fixed...
        declare -a macos_plugins=("Plugins/platforms/libqcocoa.dylib")

        for macos_lib in "${macos_plugins[@]}"
        do
               install_name_tool -id @executable_path/../$macos_lib "citra-qt.app/Contents/$macos_lib"
               for macos_lib2 in "${macos_libs[@]}"
               do
                   install_name_tool -change $QT_BREWS_PATH/lib/$macos_lib2.framework/Versions/$QT_VERSION_NUM/$macos_lib2 @executable_path/../Frameworks/$macos_lib2.framework/Versions/$QT_VERSION_NUM/$macos_lib2 "citra-qt.app/Contents/$macos_lib"
                   install_name_tool -change "$BREW_PATH/opt/qt5/lib/$macos_lib2.framework/Versions/$QT_VERSION_NUM/$macos_lib2" @executable_path/../Frameworks/$macos_lib2.framework/Versions/$QT_VERSION_NUM/$macos_lib2 "citra-qt.app/Contents/$macos_lib"
               done
        done

        for macos_lib in "${macos_libs[@]}"
        do
            # Debugging info for Travis-CI
            otool -L "citra-qt.app/Contents/Frameworks/$macos_lib.framework/Versions/$QT_VERSION_NUM/$macos_lib"
        done
        
        # Make the citra-qt.app application launch a debugging terminal.
        # Store away the actual binary
        mv citra-qt.app/Contents/MacOS/citra-qt citra-qt.app/Contents/MacOS/citra-qt-bin

        cat > citra-qt.app/Contents/MacOS/citra-qt <<EOL
#!/usr/bin/env bash
cd "\`dirname "\$0"\`"
chmod +x citra-qt-bin
open citra-qt-bin
EOL # Content that will serve as the launching script for citra (within the .app folder)

        # Make the launching script executable
        chmod +x citra-qt.app/Contents/MacOS/citra-qt

    fi

    # Copy documentation
    cp license.txt "$REV_NAME"
    cp README.md "$REV_NAME"

    ARCHIVE_NAME="${REV_NAME}.tar.xz"
    tar -cJvf "$ARCHIVE_NAME" "$REV_NAME"
    lftp -c "open -u citra-builds,$BUILD_PASSWORD sftp://builds.citra-emu.org; set sftp:auto-confirm yes; put -O '$UPLOAD_DIR' '$ARCHIVE_NAME'"
fi
