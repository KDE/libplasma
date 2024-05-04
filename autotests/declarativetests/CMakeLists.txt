# SPDX-FileCopyrightText: 2024 Fushan Wen <qydwhotmail@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

add_executable(qmltest qmltest.cpp)
target_link_libraries(qmltest Qt::QuickTest Qt::Qml)

add_test(
    NAME bug485688test
    COMMAND qmltest -import ${CMAKE_BINARY_DIR}/bin -input "${CMAKE_CURRENT_SOURCE_DIR}/bug485688.qml"
)

add_test(
    NAME componentstest
    COMMAND qmltest -import ${CMAKE_BINARY_DIR}/bin -input "${CMAKE_CURRENT_SOURCE_DIR}/components.qml"
)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set_tests_properties(componentstest PROPERTIES ENVIRONMENT "QT_FATAL_WARNINGS=1")
endif()
