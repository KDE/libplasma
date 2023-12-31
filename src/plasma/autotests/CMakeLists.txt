set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

include_directories ( ${karchive_SOURCE_DIR}/src
                      ${karchive_BINARY_DIR}/src
)

find_package(KCoreAddons REQUIRED)

MACRO(PLASMA_UNIT_TESTS)
       FOREACH(_testname ${ARGN})
               kde4_add_unit_test(${_testname} TESTNAME plasma-${_testname} ${_testname}.cpp)
               target_link_libraries(${_testname} ${QT_QTTEST_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY}
                                     ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS} plasma ${KArchive_LIBRARIES} ${KCoreAddons_LIBRARIES})
               if(QT_QTOPENGL_FOUND)
                   target_link_libraries(${_testname} ${QT_QTOPENGL_LIBRARY})
               endif(QT_QTOPENGL_FOUND)
       ENDFOREACH(_testname)
ENDMACRO(PLASMA_UNIT_TESTS)

PLASMA_UNIT_TESTS(
    configloadertest
    packagestructuretest
    #    plasmoidpackagetest
    runnercontexttest
)

kde4_add_unit_test(storagetest TESTNAME plasma-storage storagetest.cpp ../private/storage.cpp ../private/storagethread.cpp)
target_link_libraries(storagetest ${QT_QTTEST_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTSQL_LIBRARY} ${KDE4_KIO_LIBS} plasma ${KCoreAddons_LIBRARIES} )
if(QT_QTOPENGL_FOUND)
   target_link_libraries(storagetest ${QT_QTOPENGL_LIBRARY})
endif(QT_QTOPENGL_FOUND)

if(QCA2_FOUND)
   #  target_link_libraries(plasmoidpackagetest ${QCA2_LIBRARIES})
endif(QCA2_FOUND)

