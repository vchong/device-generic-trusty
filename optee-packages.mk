# OP TEE client library and service
PRODUCT_PACKAGES += libteec \
                    tee-supplicant \
                    libckteec

# optee_test and TA
PRODUCT_PACKAGES += xtest

# optee_test supp_plugin
PRODUCT_PACKAGES += f07bfc66-958c-4a15-99c0-260e4e7375dd.plugin

# optee_example syslog plugin
PRODUCT_PACKAGES += 96bcf744-4f72-4866-bf1d-8634fd9c65e5.plugin

# optee examples
PRODUCT_PACKAGES += optee_example_hello_world
PRODUCT_PACKAGES += optee_example_random
PRODUCT_PACKAGES += optee_example_aes
PRODUCT_PACKAGES += optee_example_hotp
PRODUCT_PACKAGES += optee_example_acipher
PRODUCT_PACKAGES += optee_example_secure_storage
PRODUCT_PACKAGES += optee_example_plugins

# gl km & gk TA
PRODUCT_PACKAGES += wait_for_keymaster_optee
PRODUCT_PACKAGES += KMGK_gtest
PRODUCT_PACKAGES += android.hardware.keymaster@3.0-service.optee
PRODUCT_PACKAGES += android.hardware.gatekeeper@1.0-service.optee
