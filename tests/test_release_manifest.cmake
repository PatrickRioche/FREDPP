if(NOT DEFINED FREDPP_SOURCE_DIR OR NOT DEFINED FREDPP_VERSION)
    message(FATAL_ERROR "FREDPP_SOURCE_DIR and FREDPP_VERSION are required")
endif()

if(NOT FREDPP_VERSION MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
    message(FATAL_ERROR "Invalid release version: ${FREDPP_VERSION}")
endif()

set(required_files
    LICENSE
    NOTICE
    CHANGELOG.md
    ROADMAP.md
    RELEASE_NOTES.md
    packaging/LISEZMOI-WINDOWS.txt
    packaging/LISEZMOI-DEBIAN.txt
    packaging/LISEZMOI-MACOS.txt
    library/aide.fredpp
    library/hello.fredpp
    library/index.fredpp
    library/ouya.fredpp
    scripts/package-release.ps1
    scripts/package-release.sh
    scripts/package-release-macos.sh
    editors/vscode/package.json
    editors/vscode/language-configuration.json
    editors/vscode/syntaxes/fredpp.tmLanguage.json
    editors/vscode/snippets/fredpp.json
    editors/vscode/README.md
    assets/branding/README.md
    assets/branding/fredpp-logo-reference.png
    assets/branding/fredpp-android-icon.png
    android/settings.gradle.kts
    android/build.gradle.kts
    android/gradle.properties
    android/gradlew
    android/gradlew.bat
    android/gradle/wrapper/gradle-wrapper.jar
    android/gradle/wrapper/gradle-wrapper.properties
    android/app/build.gradle.kts
    android/app/src/main/AndroidManifest.xml
    android/app/src/main/cpp/CMakeLists.txt
    android/app/src/main/cpp/native-lib.cpp
    android/app/src/main/java/fr/fredpp/android/MainActivity.kt
    android/app/src/main/java/fr/fredpp/android/NativeBridge.kt
    android/app/src/main/res/drawable-nodpi/fredpp_logo.png
    android/app/src/main/res/mipmap-xxxhdpi/ic_launcher.png
    .github/workflows/android-ci.yml
    scripts/release-stats.ps1
    scripts/release-stats.sh
    .github/workflows/release.yml
)

foreach(relative_path IN LISTS required_files)
    if(NOT EXISTS "${FREDPP_SOURCE_DIR}/${relative_path}")
        message(FATAL_ERROR "Missing release file: ${relative_path}")
    endif()
endforeach()
