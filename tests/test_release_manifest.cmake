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
    scripts/package-release.ps1
    scripts/package-release.sh
    editors/vscode/package.json
    editors/vscode/language-configuration.json
    editors/vscode/syntaxes/fredpp.tmLanguage.json
    editors/vscode/snippets/fredpp.json
    editors/vscode/README.md
    scripts/release-stats.ps1
    scripts/release-stats.sh
    .github/workflows/release.yml
)

foreach(relative_path IN LISTS required_files)
    if(NOT EXISTS "${FREDPP_SOURCE_DIR}/${relative_path}")
        message(FATAL_ERROR "Missing release file: ${relative_path}")
    endif()
endforeach()
