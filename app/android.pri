android {
    message("Building ANDROID")
    message("ANDROID Platform: $${ANDROID_TARGET_ARCH}")

    DEFINES += MOBILE_OS

    isEmpty(QGIS_INSTALL_PATH) {
      error("Missing QGIS_INSTALL_PATH")
    }

    # using installed QGIS
    QGIS_PREFIX_PATH = $${QGIS_INSTALL_PATH}
    QGIS_LIB_DIR = $${QGIS_INSTALL_PATH}/lib
    QGIS_INCLUDE_DIR = $${QGIS_INSTALL_PATH}/include/qgis

    exists($${QGIS_LIB_DIR}/libqgis_core_$${ANDROID_TARGET_ARCH}.so) {
      message("Building from QGIS: $${QGIS_LIB_DIR}/libqgis_core_$${ANDROID_TARGET_ARCH}.so")
    } else {
      error("Missing QGIS Core library in $${QGIS_LIB_DIR}/libqgis_core_$${ANDROID_TARGET_ARCH}.so")
    }

    isEmpty(QGSQUICK_INSTALL_PATH) {
      error("Missing QGSQUICK_INSTALL_PATH")
    }

    INCLUDEPATH += $${QGIS_INCLUDE_DIR}
    LIBS += -L$${QGIS_LIB_DIR}
    LIBS += -lqgis_core_$${ANDROID_TARGET_ARCH}

    # using installed QGSQUICK
    QGSQUICK_LIB_DIR = $${QGSQUICK_INSTALL_PATH}/lib
    QGSQUICK_INCLUDE_DIR = $${QGSQUICK_INSTALL_PATH}/include
    exists($${QGSQUICK_LIB_DIR}/libqgis_quick_$${ANDROID_TARGET_ARCH}.so) {
      message("Building from QGIS: $${QGSQUICK_LIB_DIR}/libqgis_quick_$${ANDROID_TARGET_ARCH}.so")
    } else {
      error("Missing QGIS Quick library in $${QGSQUICK_LIB_DIR}/libqgis_quick_$${ANDROID_TARGET_ARCH}.so")
    }

    QGSQUICK_QML_DIR = $${QGSQUICK_INSTALL_PATH}/qml
    exists($${QGSQUICK_QML_DIR}/QgsQuick/qmldir) {
      message("Building from QgsQuick plugin: $${QGSQUICK_QML_DIR}/QgsQuick")
    } else {
      error("Missing QgsQuick plugin in $${QGSQUICK_QML_DIR}/QgsQuick")
    }

    INCLUDEPATH += $${QGSQUICK_INCLUDE_DIR}
    LIBS += -L$${QGSQUICK_LIB_DIR}
    LIBS += -lqgis_quick_$${ANDROID_TARGET_ARCH}

    # Geodiff
    INCLUDEPATH += $${GEODIFF_INCLUDE_DIR}
    LIBS += -L$${GEODIFF_LIB_DIR}
    LIBS += -lgeodiff

    QT += printsupport
    QT += androidextras

    QMAKE_CXXFLAGS += -std=c++11

    # files from this folder will be added to the package
    # (and will override any default files from Qt - template is in $QTDIR/src/android)
    system($$PWD/../scripts/patch_manifest.bash $${ANDROID_VERSION_NAME} $${ANDROID_VERSION_CODE} $$PWD/android $$OUT_PWD/android_patched)
    ANDROID_PACKAGE_SOURCE_DIR = $$OUT_PWD/android_patched
    QT_LIBS_DIR = $$dirname(QMAKE_QMAKE)/../lib

    # this makes the manifest visible from Qt Creator
    DISTFILES += $$OUT_PWD/android_patched/AndroidManifest.xml
    DISTFILES += $$OUT_PWD/res/xml/file_paths.xml
    DISTFILES += $$PWD/build.gradle

    # packaging
    ANDROID_EXTRA_LIBS += \
        $${QGIS_LIB_DIR}/libcrypto_1_1.so \
        $${QGIS_LIB_DIR}/libpng16.so \
        $${QGIS_LIB_DIR}/libexpat.so \
        $${QGIS_LIB_DIR}/libgeodiff.so \
        $${QGIS_LIB_DIR}/libgeos.so \
        $${QGIS_LIB_DIR}/libgeos_c.so \
        $${QGIS_LIB_DIR}/libsqlite3.so \
        $${QGIS_LIB_DIR}/libcharset.so \
        $${QGIS_LIB_DIR}/libiconv.so \
        $${QGIS_LIB_DIR}/libfreexl.so \
        $${QGIS_LIB_DIR}/libgdal.so \
        $${QGIS_LIB_DIR}/libproj.so \
        $${QGIS_LIB_DIR}/libspatialindex.so \
        $${QGIS_LIB_DIR}/libpq.so \
        $${QGIS_LIB_DIR}/libspatialite.so \
        $${QGIS_LIB_DIR}/libprotobuf-lite.so \
        $${QGIS_LIB_DIR}/libqca-qt5_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libqgis_core_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libqgis_native_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libqt5keychain_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libzip.so \
        $${QGIS_LIB_DIR}/libspatialiteprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libdelimitedtextprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libgpxprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libmssqlprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libowsprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libpostgresprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libspatialiteprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libssl_1_1.so \
        $${QGIS_LIB_DIR}/libwcsprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libwfsprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGIS_LIB_DIR}/libwmsprovider_$${ANDROID_TARGET_ARCH}.so \
        $${QGSQUICK_LIB_DIR}/libqgis_quick_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5OpenGL_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5PrintSupport_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Sensors_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Network_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Sql_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5Svg_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5AndroidExtras_$${ANDROID_TARGET_ARCH}.so \
        $$QT_LIBS_DIR/libQt5SerialPort_$${ANDROID_TARGET_ARCH}.so

    ANDROID_EXTRA_PLUGINS += $${QGSQUICK_QML_DIR}
}
