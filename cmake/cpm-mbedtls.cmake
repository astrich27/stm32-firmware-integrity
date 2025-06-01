CPMAddPackage(
        NAME MbedTLS
        GITHUB_REPOSITORY Mbed-TLS/mbedtls
        GIT_TAG v3.4.0
        OPTIONS
        "ENABLE_PROGRAMS OFF"
        "ENABLE_TESTING OFF"
)

target_compile_definitions(mbedcrypto PUBLIC MBEDTLS_CONFIG_FILE="${CMAKE_CURRENT_LIST_DIR}/mbedtls_config.h")
target_compile_definitions(mbedtls    PUBLIC MBEDTLS_CONFIG_FILE="${CMAKE_CURRENT_LIST_DIR}/mbedtls_config.h")
target_compile_definitions(mbedx509   PUBLIC MBEDTLS_CONFIG_FILE="${CMAKE_CURRENT_LIST_DIR}/mbedtls_config.h")