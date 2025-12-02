cmake_minimum_required(VERSION 3.10)
project(lab06)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---------- POBIERANIE FLAG BEZPOŚREDNIO Z wx-config ----------
# Używamy ścieżki /usr/bin/wx-config, którą wcześniej zweryfikowaliśmy

# 1. Pobierz flagi definicji makr (-D)
execute_process(COMMAND /usr/bin/wx-config --cxxflags OUTPUT_VARIABLE WX_CXX_DEFS)

# 2. Pobierz flagi ścieżek dołączania (-I)
execute_process(COMMAND /usr/bin/wx-config --cflags OUTPUT_VARIABLE WX_INCLUDE_FLAGS)

# 3. Pobierz flagi linkowania (biblioteki core, base, aui)
execute_process(COMMAND /usr/bin/wx-config --libs core base aui OUTPUT_VARIABLE WX_LIBS)

# Usuń znaki końca linii
string(STRIP "${WX_CXX_DEFS}" WX_CXX_DEFS)
string(STRIP "${WX_INCLUDE_FLAGS}" WX_INCLUDE_FLAGS)
string(STRIP "${WX_LIBS}" WX_LIBS)

# ----------------------------------------------------------------------

# Definicja wszystkich plików źródłowych
set(SOURCE_FILES 
    main.cpp
    vecmat.cpp
    GUIMyFrame1.cpp
    GUI.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# 4. Zastosuj flagi definicji makr (-D__WXGTK__), które usuwają błąd "No Target!"
separate_arguments(WX_COMPILE_LIST UNIX_COMMAND ${WX_CXX_DEFS})
target_compile_options(${PROJECT_NAME} PRIVATE ${WX_COMPILE_LIST})

# 5. Zastosuj ścieżki dołączania (-I) - niezbędne, by kompilator znalazł nagłówki wxWidgets i nagłówki projektu (np. GUI.h)
separate_arguments(WX_INCLUDE_LIST UNIX_COMMAND ${WX_INCLUDE_FLAGS})

# Iterujemy i usuwamy prefiks -I, aby target_include_directories poprawnie przetwarzało ścieżki
set(WX_CLEAN_INCLUDES "")
foreach(ITEM ${WX_INCLUDE_LIST})
    if (ITEM MATCHES "^-I")
        string(SUBSTRING ${ITEM} 2 -1 WX_PATH)
        list(APPEND WX_CLEAN_INCLUDES ${WX_PATH})
    else()
        list(APPEND WX_CLEAN_INCLUDES ${ITEM})
    endif()
endforeach()

# Dodajemy nagłówki wxWidgets i lokalny katalog nagłówków
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${WX_CLEAN_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR} # Aby kompilator znalazł GUI.h, GUIMyFrame1.h, itp.
)

# 6. Zastosuj flagi linkowania
separate_arguments(WX_LINK_LIST UNIX_COMMAND ${WX_LIBS})
target_link_libraries(${PROJECT_NAME} ${WX_LINK_LIST})